#include "directionallightshader.h"

DirectionalLightShader::DirectionalLightShader()
{
	Utility::Resource res("shaders");

	MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
	Shader vert(Version::GL330, Shader::Type::Vertex);
	vert.addSource(res.get("directionallightshader.vert"));
	CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
	attachShader(vert);

	MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
	Shader frag(Version::GL330, Shader::Type::Fragment);
	frag.addSource(res.get("directionallightshader.frag"));
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

	setUniform(uniform_Position, m_PosLayer);
	setUniform(uniform_Normals, m_NormalLayer);
	setUniform(uniform_Diffuse, m_DiffuseLayer);
}

DirectionalLightShader& DirectionalLightShader::setP(const Matrix4& p)
{
	setUniform(uniform_P, p);
	return *this;
}

DirectionalLightShader& DirectionalLightShader::setMV(const Matrix4& mv)
{
	setUniform(uniform_MV, mv);
	return *this;
}

DirectionalLightShader& DirectionalLightShader::setWorldTransformation(const Vector3& wt)
{
	setUniform(uniform_WorldTransformation, wt);
	return *this;
}

DirectionalLightShader& DirectionalLightShader::setGBuffer(const Vector2& screenSize, Texture2D& pos, Texture2D& normals, Texture2D& diffuse)
{
	pos.bind(m_PosLayer);
	normals.bind(m_NormalLayer);
	diffuse.bind(m_DiffuseLayer);
	setUniform(uniform_ScreenSize, screenSize);
	return *this;
}
