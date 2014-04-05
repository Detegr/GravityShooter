#include "main.h"
#include "ball.h"

Ball::Ball(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "ball", "flat") {}

void Ball::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	if(!m_ActiveShader) switchShader();
	CORRADE_INTERNAL_ASSERT(m_FlatShader);
	CORRADE_INTERNAL_ASSERT(m_DeferredShader);
	CORRADE_INTERNAL_ASSERT(m_ActiveShader);

	if(m_ActiveShader == &(*m_DeferredShader))
		m_DeferredShader->setP(cam.projectionMatrix()).setMV(trans);
	else
		m_FlatShader->setColor({0.3f, 0.3f, 1.0f, 1.0f})
			.setTransformationProjectionMatrix(cam.projectionMatrix() * trans);
	m_Mesh->draw(*m_ActiveShader);
}

