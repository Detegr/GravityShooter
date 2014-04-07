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
	protected:
		float LinearAttenuation;
		float ExponentialAttenuation;
	public:
		PointLight(Object3D* parent, SceneGraph::DrawableGroup3D* group);
		void draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam) override;
		void Initialize(Vector3 color, float linear, float exp, float diffuse);
};

