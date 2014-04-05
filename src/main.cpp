#include "main.h"
#include "loaders.h"
#include "plane.h"
#include "ball.h"

static void BuildMap(Object3D* mapParent, SceneGraph::DrawableGroup3D* group)
{
	(new Plane(mapParent, group))->scale({80.0f, 40.0f, 1.0f}).translate(Vector3::zAxis(-280.0f)); // Back
	(new Plane(mapParent, group))->scale({80.0f, 40.0f, 1.0f}).rotateX(Deg(180)).translate(Vector3::zAxis(80.0f)); // Front

	(new Plane(mapParent, group))->scale({80.0f, 40.0f, 1.0f}).rotateY(Deg(-90)).translate({80.0f, 0.0f, 0.0f}); // Left
	(new Plane(mapParent, group))->scale({80.0f, 40.0f, 1.0f}).rotateY(Deg(90)).translate({-80.0f, 0.0f, 0.0f}); // Left

	(new Plane(mapParent, group))->scale({80.0f, 80.0f, 1.0f}).rotateX(Deg(-90)).translate({0.0f, -40.0f, 0.0f});
	(new Plane(mapParent, group))->scale({80.0f, 80.0f, 1.0f}).rotateX(Deg(90)).translate({0.0f, 40.0f, 0.0f});

	for(int i=0; i<100; i+=20)
	{
		(new Ball(mapParent, group))->scale({10.0f, 10.0f, 10.0f}).translate({70.0f-i, -30.0f + (i/2), -70.0f + i});
	}
}

GravityShooter::GravityShooter(const Arguments& args) : Platform::Application(args, Configuration()), m_BufferImage(ColorFormat::Red, ColorType::UnsignedByte),
	m_DepthFrameBuffer(defaultFramebuffer.viewport())
{
	PluginManager::Manager<Trade::AbstractImporter> pluginManager(MAGNUM_PLUGINS_IMPORTER_DIR);
	if(!(pluginManager.load("JpegImporter") & PluginManager::LoadState::Loaded))
	{
		//Error() << "No JpegImporter present";
		std::exit(1);
	}
	m_Manager.set("jpegimporter", pluginManager.instance("JpegImporter").release());

	m_DepthFrameBuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Depth, m_DepthBuffer);

	srand(time(nullptr));
	setMouseLocked(true);
	(m_CameraObject = new Object3D(&m_Scene));
	(m_Camera = new SceneGraph::Camera3D(*m_CameraObject))
		->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
		.setPerspective(Deg(35.0f), 1.0f, 0.001f, 500)
		.setViewport(defaultFramebuffer.viewport().size());
	Renderer::setFeature(Renderer::Feature::DepthTest, true);
	Renderer::setFeature(Renderer::Feature::FaceCulling, true);
	Renderer::setFeature(Renderer::Feature::Blending, true);

	//m_Manager.set("phong", new Shaders::(Shaders::Phong::Flag::DiffuseTexture));
	m_Manager.set("flat", new Shaders::Flat3D);
	m_Manager.set("flattextured", new Shaders::Flat3D(Shaders::Flat3D::Flag::Textured));
	m_Manager.set("deferred1stpass", new Deferred1stPass);
	m_Manager.setLoader<Trade::MeshData3D>(new PrimitiveLoader)
		.setLoader<Mesh>(new MeshLoader)
		.setLoader<Texture2D>(new TexLoader);

	m_CameraUp=m_CameraObject->transformation().up();
	m_CameraRight=m_CameraObject->transformation().right();
	m_CameraBack=m_CameraObject->transformation().backward();
	m_RootObject = new Object3D(&m_Scene);
	BuildMap(m_RootObject, &m_MapDrawables);

	m_Manager.clear<Trade::AbstractImporter>();
}

bool GravityShooter::keyPressed(KeyEvent::Key k) const
{
	try {return m_Keys.at(static_cast<SDL_Keycode>(k)).first; }
	catch(const std::out_of_range& e) { return false; }
}
bool GravityShooter::keyHit(KeyEvent::Key k)
{
	try {
		if(!m_Keys.at(static_cast<SDL_Keycode>(k)).second)
		{
			m_Keys[static_cast<SDL_Keycode>(k)].second=true;
			return true;
		}
		return false;
	}
	catch(const std::out_of_range& e) { return false; }
}

void GravityShooter::drawEvent()
{
	defaultFramebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth);

	if(keyPressed(KeyEvent::Key::W))
		m_RootObject->translate(m_CameraObject->transformation().backward(), SceneGraph::TransformationType::Local);
	else if(keyPressed(KeyEvent::Key::S))
		m_RootObject->translate(-m_CameraObject->transformation().backward(), SceneGraph::TransformationType::Local);
	if(keyPressed(KeyEvent::Key::A))
		m_RootObject->translate(m_CameraObject->transformation().right(), SceneGraph::TransformationType::Local);
	else if(keyPressed(KeyEvent::Key::D))
		m_RootObject->translate(-m_CameraObject->transformation().right(), SceneGraph::TransformationType::Local);

	defaultFramebuffer.bind(FramebufferTarget::Draw);
	m_Camera->draw(m_MapDrawables);
	swapBuffers();

	if(keyPressed(KeyEvent::Key::Space))
	{
		m_DepthFrameBuffer.clear(FramebufferClear::Color | FramebufferClear::Depth);
		m_DepthFrameBuffer.bind(FramebufferTarget::Draw);
		m_Camera->draw(m_MapDrawables);

		m_DepthFrameBuffer.read({0, 0}, m_Camera->viewport(), m_BufferImage, BufferUsage::StaticDraw);
		auto tex = new Texture2D;
		tex->setWrapping(Sampler::Wrapping::ClampToEdge)
			.setMinificationFilter(Sampler::Filter::Nearest)
			.setMagnificationFilter(Sampler::Filter::Nearest)
			.setImage(0, TextureFormat::RGB8, m_BufferImage);
		//m_Manager.set("rendered", tex, );
		auto newplane = new Plane(m_RootObject, &m_MapDrawables);
		newplane->setTexture(tex);
		newplane->scale({20.0f, 20.0f, 1.0f}).translate(-m_RootObject->transformation().translation());
	}

	if(keyHit(KeyEvent::Key::Q))
	{
		for(Object3D* o=m_RootObject->firstChild(); o; o=o->nextSibling())
		{
			MapObject* mo = dynamic_cast<MapObject*>(o);
			mo->switchShader();
		}
	}
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
	m_Keys[static_cast<SDL_Keycode>(e.key())] = std::make_pair(true, false);
}
void GravityShooter::keyReleaseEvent(KeyEvent& e)
{
	m_Keys[static_cast<SDL_Keycode>(e.key())] = std::make_pair(false, true);
}

MAGNUM_APPLICATION_MAIN(GravityShooter)
