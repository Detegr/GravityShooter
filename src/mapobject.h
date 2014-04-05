#pragma once

#include "main.h"

using namespace Magnum;
class MapObject : public Object3D, SceneGraph::Drawable3D
{
	protected:
		MapObject(Object3D* parent,SceneGraph::DrawableGroup3D* group, const std::string& meshName, const std::string& shaderName) :
			Object3D(parent), SceneGraph::Drawable3D(*this, group),
			m_Mesh(GravityShooterResourceManager::instance().get<Mesh>(meshName)),
			m_FlatShader(GravityShooterResourceManager::instance().get<Shaders::Flat3D>(shaderName))
			{}

		Resource<Mesh> m_Mesh;
		Resource<Shaders::Flat3D> m_FlatShader;
};

