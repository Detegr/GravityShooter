#pragma once

#include "main.h"
#include "mapobject.h"
#include "baselight.h"

using namespace Magnum;
class PointLight : public MapObject, public BaseLight
{
	private:
		Resource<PointLightShader> m_Shader;
		void M_SetSphereSize();
		float m_Radius;
	protected:
		float LinearAttenuation;
		float ExponentialAttenuation;
	public:
		PointLight(Object3D* parent, SceneGraph::DrawableGroup3D* group);
		void draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam) override;
		PointLight& Initialize(Vector3 color, float linear, float exp, float diffuse);
};

