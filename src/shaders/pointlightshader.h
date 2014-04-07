#pragma once

#include "../main.h"

using namespace Magnum;
class PointLightShader : public AbstractShaderProgram
{
	private:
		Int uniform_MV;
		Int uniform_P;
		Int uniform_Position;
		Int uniform_Normals;
		Int uniform_Diffuse;
		Int uniform_ScreenSize;
		Int uniform_WorldTransformation;

		enum : Int {m_PosLayer, m_NormalLayer, m_DiffuseLayer};
	public:
		explicit PointLightShader();

		PointLightShader& setP(const Matrix4& p);
		PointLightShader& setMV(const Matrix4& mv);
		PointLightShader& setGBuffer(const Vector2&, Texture2D&, Texture2D&, Texture2D&);
		PointLightShader& setWorldTransformation(const Vector3& wt);
};
