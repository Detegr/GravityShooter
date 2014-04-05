#pragma once

#include "main.h"
#include "mapobject.h"

class Ball : public MapObject
{
	public:
		Ball(Object3D* parent, SceneGraph::DrawableGroup3D* group);
		void draw(const Matrix4& trans, SceneGraph::AbstractCamera3D& cam) override;
};

