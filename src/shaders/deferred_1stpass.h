#pragma once

#include "../main.h"

using namespace Magnum;
class Deferred1stPass : public AbstractShaderProgram
{
	private:
		Int uniform_MVP;
		Int uniform_modelTranslation;
	public:
		typedef Attribute<0, Vector2> Position;
		explicit Deferred1stPass();

		Deferred1stPass& setMVP(const Matrix4& mvp);
		Deferred1stPass& setTranslationMatrix(const Matrix4& trans);
};
