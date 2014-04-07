#pragma once

#include "../main.h"

using namespace Magnum;
class Deferred1stPass : public AbstractShaderProgram
{
	private:
		Int uniform_P;
		Int uniform_MV;
		Int uniform_Tex;

		enum : Int {m_TexLayer};
	public:
		explicit Deferred1stPass();

		Deferred1stPass& setP(const Matrix4& p);
		Deferred1stPass& setMV(const Matrix4& mv);
		Deferred1stPass& setTexture(Texture2D& texture);

		enum : UnsignedInt {
			PositionOutput = 0,
			NormalOutput = 1,
			DiffuseOutput = 2
		};
};
