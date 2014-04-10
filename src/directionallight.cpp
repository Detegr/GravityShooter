#include "main.h"
#include "directionallight.h"

DirectionalLight::DirectionalLight(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "plane", "flat", "phong"),
	m_Shader(GravityShooterResourceManager::instance().get<DirectionalLightShader>("directionallight"))
{}

void DirectionalLight::draw(const Matrix4&, SceneGraph::AbstractCamera3D&)
{
	CORRADE_INTERNAL_ASSERT(m_Shader);
	auto& m = GravityShooterResourceManager::instance();
	m_Shader->setP(Matrix4()).setMV(Matrix4())
		.setGBuffer({(float)defaultFramebuffer.viewport().sizeX(), (float)defaultFramebuffer.viewport().sizeY()},
					*m.get<Texture2D>("postex"),
					*m.get<Texture2D>("normaltex"),
					*m.get<Texture2D>("difftex"));
	m_Mesh->draw(*m_Shader);
}

void DirectionalLight::Initialize(const Vector3& color)
{
	Color=color;
}
