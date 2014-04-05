#include "main.h"
#include "ball.h"

Ball::Ball(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "ball", "flat"),
	m_DeferredShader(GravityShooterResourceManager::instance().get<Deferred1stPass>("deferred1stpass")) {}

void Ball::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	m_FlatShader->setColor({0.3f, 0.3f, 1.0f, 1.0f})
		.setTransformationProjectionMatrix(cam.projectionMatrix() * trans);
	m_DeferredShader->setMVP(cam.projectionMatrix() * trans).setTranslationMatrix(parent()->absoluteTransformation() * trans);
	m_Mesh->draw(*m_DeferredShader);
}

