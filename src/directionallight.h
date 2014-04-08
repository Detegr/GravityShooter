#pragma once

#include "main.h"
#include "mapobject.h"
#include "baselight.h"

using namespace Magnum;
class DirectionalLight : public MapObject, public BaseLight
{
	private:
		Resource<DirectionalLightShader> m_Shader;
	public:
		DirectionalLight(Object3D* parent, SceneGraph::DrawableGroup3D* group);
		void draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam) override;
		void Initialize(const Vector3& color);
};

