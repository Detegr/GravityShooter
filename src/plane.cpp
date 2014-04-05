#include "main.h"
#include "plane.h"
#include <sstream>

Plane::Plane(Object3D* parent, SceneGraph::DrawableGroup3D* group)
	: MapObject(parent, group, "plane", "flattextured"),
	m_Texture(GravityShooterResourceManager::instance().get<Texture2D>("wall"))
	{}


void Plane::draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam)
{
	if(!m_ActiveShader) switchShader();
	CORRADE_INTERNAL_ASSERT(m_FlatShader);
	CORRADE_INTERNAL_ASSERT(m_DeferredShader);
	CORRADE_INTERNAL_ASSERT(m_ActiveShader);
	if(m_Texture) m_FlatShader->setTexture(*m_Texture);
	if(m_ActiveShader == &(*m_DeferredShader))
		m_DeferredShader->setP(cam.projectionMatrix()).setMV(trans);
	else
		m_FlatShader->setTransformationProjectionMatrix(cam.projectionMatrix() * trans);
	m_Mesh->draw(*m_ActiveShader);
}

void Plane::setTexture(Texture2D* tex)
{
	static int i=0;
	std::string istr;
	std::stringstream ss;
	ss << i++;
	ss >> istr;
	GravityShooterResourceManager::instance().set("rendered" + istr, tex);
	m_FlatShader = GravityShooterResourceManager::instance().get<Shaders::Flat3D>("flattextured");
	m_Texture = GravityShooterResourceManager::instance().get<Texture2D>("rendered" + istr);
	//CORRADE_INTERNAL_ASSERT(m_FlatShader);
	CORRADE_INTERNAL_ASSERT(m_Texture);
}
