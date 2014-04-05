#pragma once

#include "main.h"
#include "mapobject.h"

class Plane : public MapObject
{
	private:
		Resource<Texture2D> m_Texture;
	public:
		Plane(Object3D* parent, SceneGraph::DrawableGroup3D* group);
		void draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam) override;
		void setTexture(Texture2D* tex);
};

