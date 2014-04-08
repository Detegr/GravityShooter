#include "main.h"
#include "loaders.h"
#include "plane.h"
#include "ball.h"
#include "pointlight.h"
#include "directionallight.h"

static void BuildMap(Object3D* mapParent, SceneGraph::DrawableGroup3D* group)
{
	(new Plane(mapParent, group))->scale({80.0f, 40.0f, 1.0f}).translate(Vector3::zAxis(-80.0f)); // Back
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

static void prepareRenderTexture(Texture2D& tex, TextureFormat tf, ColorFormat cf, int components)
{
	auto viewportSize = defaultFramebuffer.viewport().sizeX() * defaultFramebuffer.viewport().sizeY() * components;
	Float* initialTexture = new Float[viewportSize];
	for(int i=0; i<viewportSize; ++i) initialTexture[i] = 1.0f;
	auto init = ImageReference2D(cf, ColorType::Float, defaultFramebuffer.viewport().size(), initialTexture);
	tex.setMinificationFilter(Sampler::Filter::Nearest)
		.setMagnificationFilter(Sampler::Filter::Nearest)
		.setWrapping(Sampler::Wrapping::ClampToEdge)
		.setStorage(0, tf, defaultFramebuffer.viewport().size())
		.setImage(0, tf, init);
	delete initialTexture;
}

GravityShooter::GravityShooter(const Arguments& args) :
	Platform::Application(args, Configuration()),
	m_MainFbo(defaultFramebuffer.viewport()),
	m_Fbo(defaultFramebuffer.viewport())
{
	PluginManager::Manager<Trade::AbstractImporter> pluginManager(MAGNUM_PLUGINS_IMPORTER_DIR);
	if(!(pluginManager.load("JpegImporter") & PluginManager::LoadState::Loaded))
	{
		//Error() << "No JpegImporter present";
		std::exit(1);
	}
	m_Manager.set("jpegimporter", pluginManager.instance("JpegImporter").release());

	m_Manager.set("postex", (m_PositionTexture = new Texture2D));
	m_Manager.set("normaltex", (m_NormalTexture = new Texture2D));
	m_Manager.set("difftex", (m_DiffuseTexture = new Texture2D));
	m_Manager.set("depthtex", (m_DepthTexture = new Texture2D));
	m_Manager.set("maintex", (m_MainTexture = new Texture2D));
	prepareRenderTexture(*m_PositionTexture, TextureFormat::RGB32F, ColorFormat::RGB, 3);
	prepareRenderTexture(*m_NormalTexture, TextureFormat::RGB32F, ColorFormat::RGB, 3);
	prepareRenderTexture(*m_DiffuseTexture, TextureFormat::RGB32F, ColorFormat::RGB, 3);
	prepareRenderTexture(*m_DepthTexture, TextureFormat::DepthComponent, ColorFormat::DepthComponent, 1);
	prepareRenderTexture(*m_MainTexture, TextureFormat::RGB32F, ColorFormat::RGB, 3);

	m_Fbo.attachTexture2D(Framebuffer::ColorAttachment(0), *m_PositionTexture, 0);
	m_Fbo.attachTexture2D(Framebuffer::ColorAttachment(1), *m_NormalTexture, 0);
	m_Fbo.attachTexture2D(Framebuffer::ColorAttachment(2), *m_DiffuseTexture, 0);
	m_Fbo.attachTexture2D(Framebuffer::BufferAttachment::Depth, *m_DepthTexture, 0);
	m_Fbo.mapForDraw({
		{Deferred1stPass::PositionOutput, Framebuffer::ColorAttachment(0)},
		{Deferred1stPass::NormalOutput, Framebuffer::ColorAttachment(1)},
		{Deferred1stPass::DiffuseOutput, Framebuffer::ColorAttachment(2)},
	});

	m_MainFbo.attachTexture2D(Framebuffer::ColorAttachment(0), *m_MainTexture, 0);
	m_MainFbo.mapForDraw({
		{Shaders::Phong::Position().Location, Framebuffer::ColorAttachment(0)},
	});

	srand(time(nullptr));
	setMouseLocked(true);
	(m_CameraObject = new Object3D(&m_Scene));
	(m_Camera = new SceneGraph::Camera3D(*m_CameraObject))
		->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
		.setPerspective(Deg(35.0f), 1.0f, 0.001f, 500)
		.setViewport(defaultFramebuffer.viewport().size());
	Renderer::setClearColor({0.0f, 0.0f, 0.0f, 1.0f});
	//Renderer::setFaceCullingMode(Renderer::PolygonFacing::Front);
	Renderer::setFeature(Renderer::Feature::FaceCulling, true);

	//m_Manager.set("phong", new Shaders::(Shaders::Phong::Flag::DiffuseTexture));
	m_Manager.set("flat", new Shaders::Flat3D);
	m_Manager.set("flattextured", new Shaders::Flat3D(Shaders::Flat3D::Flag::Textured));
	m_Manager.set("phong", new Shaders::Phong(Shaders::Phong::Flag::DiffuseTexture));
	m_Manager.set("deferred1stpass", new Deferred1stPass);
	m_Manager.set("pointlight", new PointLightShader);
	m_Manager.set("directionallight", new DirectionalLightShader);

	m_Manager.setLoader<Trade::MeshData3D>(new PrimitiveLoader)
		.setLoader<Mesh>(new MeshLoader)
		.setLoader<Texture2D>(new TexLoader);

	m_CameraUp=m_CameraObject->transformation().up();
	m_CameraRight=m_CameraObject->transformation().right();
	m_CameraBack=m_CameraObject->transformation().backward();
	m_StaticRoot = new Object3D(&m_Scene);
	m_RootObject = new Object3D(m_StaticRoot);
	m_RootObject->translate({0,0,-60});
	BuildMap(m_RootObject, &m_MapDrawables);

	m_Manager.clear<Trade::AbstractImporter>().setLoader<Texture2D>(nullptr);

	//(new PointLight(m_RootObject, &m_Lights))->Initialize({1.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 80.0f);
	(new PointLight(m_RootObject, &m_Lights))->Initialize({1.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 80.0f)
		.translate({10.0f, 20.0f, 0.0f});
	(new PointLight(m_RootObject, &m_Lights))->Initialize({0.0f, 1.0f, 0.0f}, 1.0f, 1.0f, 80.0f)
		.translate({-10.0f, -20.0f, 20.0f});
	(new PointLight(m_RootObject, &m_Lights))->Initialize({0.0f, 0.0f, 1.0f}, 1.0f, 1.0f, 80.0f)
		.translate({-40.0f, -20.0f, 0.0f});
	(new PointLight(m_RootObject, &m_Lights))->Initialize({0.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 80.0f)
		.translate({70.0f, -20.0f, 0.0f});
	(new PointLight(m_RootObject, &m_Lights))->Initialize({1.0f, 0.0f, 1.0f}, 1.0f, 1.0f, 80.0f)
		.translate({20.0f, -10.0f, -60.0f});

	(new DirectionalLight(m_RootObject, &m_Lights))->Initialize({1.0f, 1.0f, 1.0f});
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
	m_MainFbo.clear(FramebufferClear::Color|FramebufferClear::Depth);

	// G-BUFFER
	Renderer::setDepthMask(true);
	m_Fbo.clear(FramebufferClear::Color|FramebufferClear::Depth);
	Renderer::setFeature(Renderer::Feature::DepthTest, true);
	Renderer::setFeature(Renderer::Feature::Blending, false);
	m_Fbo.bind(FramebufferTarget::Draw);
	for(auto o=m_RootObject->firstChild(); o; o=o->nextSibling())
	{
		try {
			MapObject& mo=dynamic_cast<MapObject&>(*o);
			mo.setDeferred();
		} catch(const std::bad_cast& e) {}
	}

	m_Camera->draw(m_MapDrawables);
	Renderer::setDepthMask(false);
	Renderer::setFeature(Renderer::Feature::DepthTest, false);

	// LIGHTS
	defaultFramebuffer.bind(FramebufferTarget::Draw);
	Renderer::setFeature(Renderer::Feature::Blending, true);
	Renderer::setBlendEquation(Renderer::BlendEquation::Add);
	Renderer::setBlendFunction(Renderer::BlendFunction::One, Renderer::BlendFunction::One);

	for(auto o=m_RootObject->firstChild(); o; o=o->nextSibling())
	{
		try {
			MapObject& mo=dynamic_cast<MapObject&>(*o);
			mo.setPhong();
		} catch(const std::bad_cast& e) {}
	}
	m_Camera->draw(m_Lights);

	/*
	int viewx = defaultFramebuffer.viewport().sizeX();
	int viewy = defaultFramebuffer.viewport().sizeY();
	int viewcx = defaultFramebuffer.viewport().centerX();
	int viewcy = defaultFramebuffer.viewport().centerY();

	m_Fbo.mapForRead(Framebuffer::ColorAttachment(0));
	AbstractFramebuffer::blit(m_Fbo, defaultFramebuffer, defaultFramebuffer.viewport(), Range2Di({viewcx, viewcy}, {viewx, viewy}), FramebufferBlit::Color, FramebufferBlitFilter::Nearest);
	m_Fbo.mapForRead(Framebuffer::ColorAttachment(1));
	AbstractFramebuffer::blit(m_Fbo, defaultFramebuffer, defaultFramebuffer.viewport(), Range2Di({0, 0}, {viewcx, viewcy}), FramebufferBlit::Color, FramebufferBlitFilter::Nearest);
	m_Fbo.mapForRead(Framebuffer::ColorAttachment(2));
	AbstractFramebuffer::blit(m_Fbo, defaultFramebuffer, defaultFramebuffer.viewport(), Range2Di({viewcx, 0}, {viewx, viewcy}), FramebufferBlit::Color, FramebufferBlitFilter::Nearest);
	m_MainFbo.mapForRead(Framebuffer::ColorAttachment(0));
	AbstractFramebuffer::blit(m_MainFbo, defaultFramebuffer, defaultFramebuffer.viewport(), Range2Di({0, viewcy}, {viewcx, viewy}), FramebufferBlit::Color, FramebufferBlitFilter::Nearest);
	*/

	swapBuffers();

	if(keyPressed(KeyEvent::Key::W))
		m_RootObject->translate(m_CameraObject->transformation().backward(), SceneGraph::TransformationType::Local);
	else if(keyPressed(KeyEvent::Key::S))
		m_RootObject->translate(-m_CameraObject->transformation().backward(), SceneGraph::TransformationType::Local);
	if(keyPressed(KeyEvent::Key::A))
		m_RootObject->translate(m_CameraObject->transformation().right(), SceneGraph::TransformationType::Local);
	else if(keyPressed(KeyEvent::Key::D))
		m_RootObject->translate(-m_CameraObject->transformation().right(), SceneGraph::TransformationType::Local);

	/*
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
	}*/
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

	if(keyHit(KeyEvent::Key::Q))
	{
		static int texnum=0;
		for(auto o=m_RootObject->firstChild(); o; o=o->nextSibling())
		{
			try {
				Plane& p=dynamic_cast<Plane&>(*o);
				switch(texnum)
				{
					case 0: p.setTexture("postex"); break;
					case 1: p.setTexture("normaltex"); break;
					case 2: p.setTexture("difftex"); break;
					case 3: p.setTexture("depthtex"); break;
					case 4: p.setTexture("wall"); break;
				}
			} catch(const std::bad_cast& e) {}
		}
		texnum = ++texnum % 5;
	}
	e.setAccepted(true);
}
void GravityShooter::keyReleaseEvent(KeyEvent& e)
{
	m_Keys[static_cast<SDL_Keycode>(e.key())] = std::make_pair(false, true);
	e.setAccepted(true);
}

MAGNUM_APPLICATION_MAIN(GravityShooter)
