#include "main.h"
#include "plane.h"
#include <sstream>

Plane::Plane(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "plane", "flattextured", "phong"),
	m_Texture(GravityShooterResourceManager::instance().get<Texture2D>("wall")),
	m_WallTexture(GravityShooterResourceManager::instance().get<Texture2D>("wall"))
	{}


void Plane::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	if(!m_ActiveShader) setPhong();
	CORRADE_INTERNAL_ASSERT(m_FlatShader);
	CORRADE_INTERNAL_ASSERT(m_DeferredShader);
	CORRADE_INTERNAL_ASSERT(m_ActiveShader);
	if(m_ActiveShader == &(*m_DeferredShader))
		m_DeferredShader->setP(cam.projectionMatrix()).setMV(trans).setTexture(*m_WallTexture);
	else
	{
		//m_FlatShader->setTransformationProjectionMatrix(cam.projectionMatrix() * trans);
		//
		Vector3 light={0.0f, 0.0f, 40.0f};
		auto pos = cam.cameraMatrix().transformPoint(parent()->transformationMatrix().translation() + light);
		m_PhongShader->setDiffuseTexture(*m_Texture)
			.setSpecularColor({0.0f, 0.0f, 0.0f})
			.setTransformationMatrix(trans)
			.setNormalMatrix(trans.rotationScaling())
			.setProjectionMatrix(cam.projectionMatrix())
			.setLightPosition(pos)
			.setShininess(1.0f);
	}
	m_Mesh->draw(*m_ActiveShader);
}

void Plane::setTexture(const std::string& texName)
{
	m_Texture = GravityShooterResourceManager::instance().get<Texture2D>(texName);
	CORRADE_INTERNAL_ASSERT(m_Texture);
}
