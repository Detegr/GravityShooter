#include "main.h"
#include "pointlight.h"
#include <sstream>
#include <cmath>

PointLight::PointLight(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "ball", "flat", "phong"),
	m_Shader(GravityShooterResourceManager::instance().get<PointLightShader>("pointlight"))
{}

void PointLight::M_SetSphereSize()
{
	float s = (-LinearAttenuation +
			sqrtf(LinearAttenuation * LinearAttenuation - 4*ExponentialAttenuation
				* (ExponentialAttenuation - 256 * Color.max() * DiffuseIntensity)))
			/ 2 * ExponentialAttenuation;
	this->scale({s, s, s});
	Debug() << "Light radius now: " << s;
}

void PointLight::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	CORRADE_INTERNAL_ASSERT(m_Shader);
	auto& m = GravityShooterResourceManager::instance();
	m_Shader->setP(cam.projectionMatrix())
		.setMV(trans)
		.setWorldTransformation(cam.cameraMatrix().transformPoint(parent()->transformation().translation()) + transformation().translation())
		.setGBuffer({(float)defaultFramebuffer.viewport().sizeX(), (float)defaultFramebuffer.viewport().sizeY()},
					*m.get<Texture2D>("postex"),
					*m.get<Texture2D>("normaltex"),
					*m.get<Texture2D>("difftex"));
	m_Mesh->draw(*m_Shader);
}

void PointLight::Initialize(Vector3 color, float linear, float exp, float diffuse)
{
	Color=color;
	LinearAttenuation=linear;
	ExponentialAttenuation=exp;
	DiffuseIntensity=diffuse;
	M_SetSphereSize();
}
