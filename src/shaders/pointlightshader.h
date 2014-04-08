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

		Int uniform_LightColor;
		Int uniform_AmbientIntensity;
		Int uniform_DiffuseIntensity;
		Int uniform_SpecularIntensity;
		Int uniform_SpecularPower;
		Int uniform_LinearAttenuation;
		Int uniform_ExponentialAttenuation;

		enum : Int {m_PosLayer, m_NormalLayer, m_DiffuseLayer};
	public:
		explicit PointLightShader();

		PointLightShader& setP(const Matrix4& p);
		PointLightShader& setMV(const Matrix4& mv);
		PointLightShader& setGBuffer(const Vector2&, Texture2D&, Texture2D&, Texture2D&);
		PointLightShader& setWorldTransformation(const Vector3& wt);
		PointLightShader& setLightData(const Vector3&, float, float, float, float, float, float);
};
