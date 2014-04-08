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
	m_Radius = (-LinearAttenuation +
			sqrtf(LinearAttenuation * LinearAttenuation - 4*ExponentialAttenuation
				* (ExponentialAttenuation - 256 * Color.max() * DiffuseIntensity)))
			/ 2 * ExponentialAttenuation;
	this->scale({m_Radius, m_Radius, m_Radius});
	Debug() << "Light radius now: " << m_Radius;
}

void PointLight::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	CORRADE_INTERNAL_ASSERT(m_Shader);
	auto& m = GravityShooterResourceManager::instance();
	//auto distanceToCamera = cam.cameraMatrix().transformPoint(trans.translation());
	auto distanceToCamera = cam.cameraMatrix().transformVector(trans.translation());
	//Debug() << ceil(distanceToCamera.length())-0.001f << "<=" << m_Radius;
	if(distanceToCamera.length()+0.001f < m_Radius)
	{
		Renderer::setFeature(Renderer::Feature::FaceCulling, false);
		//Renderer::setFaceCullingMode(Renderer::PolygonFacing::Front);
	}
	else
	{
		Renderer::setFeature(Renderer::Feature::FaceCulling, true);
		//Renderer::setFaceCullingMode(Renderer::PolygonFacing::Back);
	}
	m_Shader->setP(cam.projectionMatrix())
		.setMV(trans)
		.setWorldTransformation(cam.cameraMatrix().transformPoint(parent()->transformation().translation() + transformation().translation()))
		.setGBuffer({(float)defaultFramebuffer.viewport().sizeX(), (float)defaultFramebuffer.viewport().sizeY()},
					*m.get<Texture2D>("postex"),
					*m.get<Texture2D>("normaltex"),
					*m.get<Texture2D>("difftex"))
		.setLightData(Color, 400.0f, DiffuseIntensity, 10.0f, 4.0f, LinearAttenuation, ExponentialAttenuation);
	m_Mesh->draw(*m_Shader);
	Renderer::setFeature(Renderer::Feature::FaceCulling, true);
}

PointLight& PointLight::Initialize(Vector3 color, float linear, float exp, float diffuse)
{
	Color=color;
	LinearAttenuation=linear;
	ExponentialAttenuation=exp;
	DiffuseIntensity=diffuse;
	M_SetSphereSize();
	return *this;
}
