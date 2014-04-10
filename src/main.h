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
#include <Magnum/Query.h>
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
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/Vector.h>
#include <Magnum/Text/GlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Texture.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Version.h>
#include <MagnumExternal/Optional/optional.hpp>
#include <MagnumPlugins/FreeTypeFont/FreeTypeFont.h>
#include <MagnumPlugins/JpegImporter/JpegImporter.h>

#include <unordered_map>
#include <string>

#include <configure.h>
#include "shaders/deferred_1stpass.h"
#include "shaders/pointlightshader.h"
#include "shaders/directionallightshader.h"

using namespace Magnum;

class Deferred1stPass;
class PointLightShader;
class DirectionalLightShader;

typedef ResourceManager<Text::FreeTypeFont, Text::GlyphCache, DirectionalLightShader, PointLightShader, Deferred1stPass, Trade::MeshData3D, Mesh, Shaders::Flat3D, Shaders::Phong, Texture2D, Trade::AbstractImporter> GravityShooterResourceManager;
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

		Shaders::Vector3D m_TextShader;
		std::unique_ptr<Text::Renderer3D> m_TextRenderer;
		PluginManager::Manager<Text::FreeTypeFont> m_FontManager;

		GravityShooterResourceManager m_Manager;

		SceneGraph::Camera3D* m_Camera;
		Vector3 m_CameraDirection;
		Vector3 m_CameraUp;
		Vector3 m_CameraRight;
		Vector3 m_CameraBack;

		Framebuffer m_MainFbo;
		Texture2D* m_MainTexture;

		Framebuffer m_Fbo;
		Texture2D* m_PositionTexture;
		Texture2D* m_NormalTexture;
		Texture2D* m_DiffuseTexture;
		Texture2D* m_DepthTexture;

		SceneGraph::DrawableGroup3D m_MapDrawables, m_Lights;
		Scene3D m_Scene;
		Object3D* m_StaticRoot, *m_RootObject, *m_CameraObject;

		std::unordered_map<SDL_Keycode, std::pair<bool, bool>> m_Keys;

		TimeQuery m_TimeQuery;
};

