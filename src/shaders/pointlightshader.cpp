#include "pointlightshader.h"

PointLightShader::PointLightShader()
{
	Utility::Resource res("shaders");

	MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
	Shader vert(Version::GL330, Shader::Type::Vertex);
	vert.addSource(res.get("pointlightshader.vert"));
	CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
	attachShader(vert);

	MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
	Shader frag(Version::GL330, Shader::Type::Fragment);
	frag.addSource(res.get("pointlightshader.frag"));
	CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
	attachShader(frag);

	CORRADE_INTERNAL_ASSERT_OUTPUT(link());

	uniform_P = uniformLocation("P");
	uniform_MV = uniformLocation("MV");
	uniform_Position = uniformLocation("posTex");
	uniform_Normals = uniformLocation("normalTex");
	uniform_Diffuse = uniformLocation("diffuseTex");
	uniform_WorldTransformation = uniformLocation("worldTransformation");
	uniform_ScreenSize = uniformLocation("screenSize");

	uniform_LightColor = uniformLocation("lightColor");
	uniform_AmbientIntensity = uniformLocation("ambientIntensity");
	uniform_DiffuseIntensity = uniformLocation("diffuseIntensity");
	uniform_SpecularIntensity = uniformLocation("specularIntensity");
	uniform_SpecularPower = uniformLocation("specularPower");
	uniform_LinearAttenuation = uniformLocation("linearAttenuation");
	uniform_ExponentialAttenuation = uniformLocation("exponentialAttenuation");

	setUniform(uniform_Position, m_PosLayer);
	setUniform(uniform_Normals, m_NormalLayer);
	setUniform(uniform_Diffuse, m_DiffuseLayer);
}

PointLightShader& PointLightShader::setP(const Matrix4& p)
{
	setUniform(uniform_P, p);
	return *this;
}

PointLightShader& PointLightShader::setMV(const Matrix4& mv)
{
	setUniform(uniform_MV, mv);
	return *this;
}

PointLightShader& PointLightShader::setWorldTransformation(const Vector3& wt)
{
	setUniform(uniform_WorldTransformation, wt);
	return *this;
}

PointLightShader& PointLightShader::setGBuffer(const Vector2& screenSize, Texture2D& pos, Texture2D& normals, Texture2D& diffuse)
{
	pos.bind(m_PosLayer);
	normals.bind(m_NormalLayer);
	diffuse.bind(m_DiffuseLayer);
	setUniform(uniform_ScreenSize, screenSize);
	return *this;
}

PointLightShader& PointLightShader::setLightData(const Vector3& lightColor, float ambient, float diffuse, float specular, float specularPower, float linear, float exponential)
{
	setUniform(uniform_LightColor, lightColor);
	setUniform(uniform_AmbientIntensity, ambient);
	setUniform(uniform_DiffuseIntensity, diffuse);
	setUniform(uniform_SpecularIntensity, specular);
	setUniform(uniform_SpecularPower, specularPower);
	setUniform(uniform_LinearAttenuation, linear);
	setUniform(uniform_ExponentialAttenuation, exponential);
	return *this;
}
