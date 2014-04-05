#pragma once

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Assert.h>
#include <Magnum/AbstractShaderProgram.h>
#include <Magnum/BufferImage.h>
#include <Magnum/Context.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Framebuffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Renderbuffer.h>
#include <Magnum/RenderbufferFormat.h>
#include <Magnum/Renderer.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/SceneGraph/Camera3D.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shader.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Texture.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Version.h>
#include <MagnumExternal/Optional/optional.hpp>
#include <MagnumPlugins/JpegImporter/JpegImporter.h>

#include <unordered_map>
#include <string>

#include <configure.h>
#include "shaders/deferred_1stpass.h"

using namespace Magnum;

class Deferred1stPass;

typedef ResourceManager<Deferred1stPass, Trade::MeshData3D, Mesh, Shaders::Flat3D, Texture2D, Trade::AbstractImporter> GravityShooterResourceManager;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

class GravityShooter : public Platform::Application
{
	public:
		explicit GravityShooter(const Arguments& args);

	private:
		void drawEvent() override;
		void viewportEvent(const Vector2i& size) override;

		void mouseMoveEvent(MouseMoveEvent& e) override;
		Vector3 positionOnSphere(const Vector2i& position) const;
		Vector3 m_PrevPos;

		void keyPressEvent(KeyEvent& e) override;
		void keyReleaseEvent(KeyEvent& e) override;
		bool keyPressed(KeyEvent::Key k) const;
		bool keyHit(KeyEvent::Key k);

		GravityShooterResourceManager m_Manager;

		SceneGraph::Camera3D* m_Camera;
		Vector3 m_CameraDirection;
		Vector3 m_CameraUp;
		Vector3 m_CameraRight;
		Vector3 m_CameraBack;

		BufferImage2D m_BufferImage;
		Framebuffer m_DepthFrameBuffer;
		Renderbuffer m_DepthBuffer;

		SceneGraph::DrawableGroup3D m_MapDrawables;
		Scene3D m_Scene;
		Object3D* m_RootObject, *m_CameraObject;

		std::unordered_map<SDL_Keycode, std::pair<bool, bool>> m_Keys;
};

