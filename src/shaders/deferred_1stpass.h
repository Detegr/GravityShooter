#pragma once

#include "../main.h"

using namespace Magnum;
class Deferred1stPass : public AbstractShaderProgram
{
	private:
		Int uniform_P;
		Int uniform_MV;
	public:
		typedef Attribute<0, Vector2> Position;
		explicit Deferred1stPass();

		Deferred1stPass& setP(const Matrix4& p);
		Deferred1stPass& setMV(const Matrix4& mv);
};
