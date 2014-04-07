#include "main.h"
#include "ball.h"

Ball::Ball(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "ball", "flat", "phong") {}

void Ball::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	if(!m_ActiveShader) setPhong();
	CORRADE_INTERNAL_ASSERT(m_FlatShader);
	CORRADE_INTERNAL_ASSERT(m_DeferredShader);
	CORRADE_INTERNAL_ASSERT(m_ActiveShader);

	if(m_ActiveShader == &(*m_DeferredShader))
		m_DeferredShader->setP(cam.projectionMatrix()).setMV(trans);
	else
	{
		/*
		m_FlatShader->setColor({0.3f, 0.3f, 1.0f, 1.0f})
			.setTransformationProjectionMatrix(cam.projectionMatrix() * trans);
		*/
		Vector3 light={0.0f, 0.0f, 40.0f};
		auto pos = cam.cameraMatrix().transformPoint(parent()->transformationMatrix().translation() + light);
		m_PhongShader->setDiffuseColor({0.3f, 0.3f, 1.0f})
			.setTransformationMatrix(trans)
			.setNormalMatrix(trans.rotationScaling())
			.setProjectionMatrix(cam.projectionMatrix())
			.setLightPosition(pos);
	}
	m_Mesh->draw(*m_ActiveShader);
}

