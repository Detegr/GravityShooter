#pragma once

#include "main.h"

using namespace Magnum;
class MapObject : public Object3D, SceneGraph::Drawable3D
{
	public:
		void setDeferred()
		{
			m_ActiveShader=&(*m_DeferredShader);
		}
		void setPhong()
		{
			m_ActiveShader=&(*m_PhongShader);
		}
	protected:
		MapObject(Object3D* parent,SceneGraph::DrawableGroup3D* group, const std::string& meshName, const std::string& shaderName, const std::string& phongShaderName) :
			Object3D(parent), SceneGraph::Drawable3D(*this, group),
			m_Mesh(GravityShooterResourceManager::instance().get<Mesh>(meshName)),
			m_FlatShader(GravityShooterResourceManager::instance().get<Shaders::Flat3D>(shaderName)),
			m_PhongShader(GravityShooterResourceManager::instance().get<Shaders::Phong>(phongShaderName)),
			m_DeferredShader(GravityShooterResourceManager::instance().get<Deferred1stPass>("deferred1stpass")),
			m_ActiveShader(nullptr)
			{}

		Resource<Mesh> m_Mesh;
		Resource<Shaders::Flat3D> m_FlatShader;
		Resource<Shaders::Phong> m_PhongShader;
		Resource<Deferred1stPass> m_DeferredShader;

		AbstractShaderProgram* m_ActiveShader;
};

