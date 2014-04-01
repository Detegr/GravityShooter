#include <Magnum/Buffer.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Renderer.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/SceneGraph/Camera3D.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData3D.h>
#include <MagnumExternal/Optional/optional.hpp>
#include <Corrade/PluginManager/Manager.h>
#include <MagnumPlugins/JpegImporter/JpegImporter.h>
#include <unordered_map>

#include <configure.h>

using namespace Magnum;

typedef ResourceManager<Trade::MeshData3D, Mesh, Shaders::Phong> GravityShooterResourceManager;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class MapObject : public Object3D, SceneGraph::Drawable3D
{
	protected:
		MapObject(Object3D* parent, SceneGraph::DrawableGroup3D* group) :
			Object3D(parent), SceneGraph::Drawable3D(*this, group),
			m_Mesh(),
			m_Shader() {}

		Resource<Mesh> m_Mesh;
		Resource<Shaders::Phong> m_Shader;
};

class MapPlane : public MapObject
{
	private:
		Object3D* m_Light;
	public:
		MapPlane(Object3D* parent, Object3D* light, SceneGraph::DrawableGroup3D* group);
		void draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam) override;
};

class GravityShooter : public Platform::Application
{
	public:
		explicit GravityShooter(const Arguments&  args);

	private:
		void drawEvent() override;
		void viewportEvent(const Vector2i& size) override;

		void mouseMoveEvent(MouseMoveEvent& e) override;
		Vector3 positionOnSphere(const Vector2i& position) const;
		Vector3 m_PrevPos;

		void keyPressEvent(KeyEvent& e) override;
		void keyReleaseEvent(KeyEvent& e) override;

		void BuildMap(Object3D* mapParent, Object3D* light, SceneGraph::DrawableGroup3D* group);

		GravityShooterResourceManager m_Manager;

		SceneGraph::Camera3D* m_Camera;
		Vector3 m_CameraDirection;
		Vector3 m_CameraUp;
		Vector3 m_CameraRight;
		Vector3 m_CameraBack;

		SceneGraph::DrawableGroup3D m_MapDrawables;
		Scene3D m_Scene;
		Object3D* m_RootObject, *m_CameraObject, *m_LightObject;

		std::unordered_map<SDL_Keycode, bool> m_Keys;
};

MapPlane::MapPlane(Object3D* parent, Object3D* light, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group), m_Light(light)
{
	auto& m = GravityShooterResourceManager::instance();
	m_Mesh = m.get<Mesh>("cube");
	m_Shader = m.get<Shaders::Phong>("phong");
}

void MapPlane::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	//Debug() << cam.cameraMatrix().backward().z() << " | " << light.x() << " " << light.y() << " " << light.z();
	auto abslight = m_Light->absoluteTransformation();
	/*
	auto lightpos = -trans + abslight;
	Debug() << abslight;
	Debug() << trans;
	Debug() << lightpos.translation();
	Debug() << abslight.translation();
	*/
	m_Shader->setAmbientColor({0.2f, 0.2f, 0.2f})
		.setDiffuseColor({0.3f, 0.3f, 0.3f})
		.setSpecularColor({0.6f, 0.6f, 0.6f})
		.setShininess(50.0f)
		.setLightPosition(abslight.translation())
		.setTransformationMatrix(trans)
		.setNormalMatrix(trans.rotationScaling())
		.setProjectionMatrix(cam.projectionMatrix());
	m_Mesh->draw(*m_Shader);
}

class PrimitiveLoader : public AbstractResourceLoader<Trade::MeshData3D>
{
	void doLoad(ResourceKey key) override
	{
		if(key == ResourceKey("plane")) set(key, Primitives::Plane::solid());
		else if(key == ResourceKey("cube")) set(key, Primitives::Cube::solid());
		else setNotFound(key);
	}
};

class MeshLoader : public AbstractResourceLoader<Mesh>
{
	void doLoad(ResourceKey key) override
	{
		auto primitive = GravityShooterResourceManager::instance().get<Trade::MeshData3D>(key);
		if(!primitive) setNotFound(key);
		else
		{
			std::optional<Mesh> mesh;
			std::unique_ptr<Buffer> buffer, indexBuffer;
			std::tie(mesh,buffer,indexBuffer) = MeshTools::compile(*primitive, BufferUsage::StaticDraw);

			// Transfer ownership to manager
			buffer.release();
			if(indexBuffer) indexBuffer.release();

			set(key, new Mesh(std::move(*mesh)));
		}
	}
};

GravityShooter::GravityShooter(const Arguments& args) : Platform::Application(args, Configuration())
{
	PluginManager::Manager<Trade::AbstractImporter> pluginManager(MAGNUM_PLUGINS_IMPORTER_DIR);
	if(!(pluginManager.load("JpegImporter") & PluginManager::LoadState::Loaded))
	{
		exit();
	}
	srand(time(nullptr));
	setMouseLocked(true);
	(m_CameraObject = new Object3D(&m_Scene));
	(m_Camera = new SceneGraph::Camera3D(*m_CameraObject))
		->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
		.setPerspective(Deg(35.0f), 1.0f, 0.001f, 500)
		.setViewport(defaultFramebuffer.viewport().size());
	Renderer::setFeature(Renderer::Feature::DepthTest, true);
	Renderer::setFeature(Renderer::Feature::FaceCulling, true);

	m_Manager.set("phong", new Shaders::Phong);
	m_Manager.setLoader<Trade::MeshData3D>(new PrimitiveLoader)
		.setLoader<Mesh>(new MeshLoader);

	m_CameraUp=m_CameraObject->transformation().up();
	m_CameraRight=m_CameraObject->transformation().right();
	m_CameraBack=m_CameraObject->transformation().backward();
	m_RootObject = new Object3D(&m_Scene);
	m_LightObject = new Object3D(m_RootObject);
	BuildMap(m_RootObject, m_LightObject, &m_MapDrawables);
}

void GravityShooter::BuildMap(Object3D* mapParent, Object3D* light, SceneGraph::DrawableGroup3D* group)
{
	(new MapPlane(mapParent, light, group))->scale({80.0f, 40.0f, 0.2f}).translate(Vector3::zAxis(-80.0f)); // Back
	(new MapPlane(mapParent, light, group))->scale({80.0f, 40.0f, 0.2f}).translate(Vector3::zAxis(80.0f)); // Front

	(new MapPlane(mapParent, light, group))->scale({80.0f, 40.0f, 0.2f}).rotateY(Deg(90)).translate({80.0f, 0.0f, 0.0f}); // Left
	(new MapPlane(mapParent, light, group))->scale({80.0f, 40.0f, 0.2f}).rotateY(Deg(-90)).translate({-80.0f, 0.0f, 0.0f}); // Left

	(new MapPlane(mapParent, light, group))->scale({80.0f, 0.2f, 80.0f}).translate({0.0f, -40.0f, 0.0f});
	(new MapPlane(mapParent, light, group))->scale({80.0f, 0.2f, 80.0f}).translate({0.0f, 40.0f, 0.0f});
}

void GravityShooter::drawEvent()
{
	defaultFramebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth);
	if(m_Keys[static_cast<SDL_Keycode>(KeyEvent::Key::A)])
		m_RootObject->translate(m_CameraObject->transformation().right(), SceneGraph::TransformationType::Local);
	else if(m_Keys[static_cast<SDL_Keycode>(KeyEvent::Key::D)])
		m_RootObject->translate(-m_CameraObject->transformation().right(), SceneGraph::TransformationType::Local);
	if(m_Keys[static_cast<SDL_Keycode>(KeyEvent::Key::W)])
		m_RootObject->translate(m_CameraObject->transformation().backward(), SceneGraph::TransformationType::Local);
	else if(m_Keys[static_cast<SDL_Keycode>(KeyEvent::Key::S)])
		m_RootObject->translate(-m_CameraObject->transformation().backward(), SceneGraph::TransformationType::Local);
	m_Camera->draw(m_MapDrawables);
	swapBuffers();

	redraw();
}
void GravityShooter::viewportEvent(const Vector2i& size)
{
	defaultFramebuffer.setViewport({{}, size});
	m_Camera->setViewport(size);
}
void GravityShooter::mouseMoveEvent(MouseMoveEvent& e)
{
	m_CameraObject->rotate(Deg(-e.relativePosition().y() * 0.1f), m_CameraRight, SceneGraph::TransformationType::Local);
	m_CameraObject->rotate(Deg(-e.relativePosition().x() * 0.1f), m_CameraUp);
}
void GravityShooter::keyPressEvent(KeyEvent& e)
{
	m_Keys[static_cast<SDL_Keycode>(e.key())] = true;
}
void GravityShooter::keyReleaseEvent(KeyEvent& e)
{
	m_Keys[static_cast<SDL_Keycode>(e.key())] = false;
}

MAGNUM_APPLICATION_MAIN(GravityShooter)
