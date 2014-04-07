#include "deferred_1stpass.h"

Deferred1stPass::Deferred1stPass()
{
	Utility::Resource res("shaders");

	MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
	Shader vert(Version::GL330, Shader::Type::Vertex);
	vert.addSource(res.get("deferred_1stpass.vert"));
	CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
	attachShader(vert);

	MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
	Shader frag(Version::GL330, Shader::Type::Fragment);
	frag.addSource(res.get("deferred_1stpass.frag"));
	CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
	attachShader(frag);

	CORRADE_INTERNAL_ASSERT_OUTPUT(link());

	uniform_P = uniformLocation("P");
	uniform_MV = uniformLocation("MV");
	uniform_Tex = uniformLocation("texSampler");
	setUniform(uniform_Tex, m_TexLayer);
}

Deferred1stPass& Deferred1stPass::setP(const Matrix4& p)
{
	setUniform(uniform_P, p);
	return *this;
}

Deferred1stPass& Deferred1stPass::setMV(const Matrix4& mv)
{
	setUniform(uniform_MV, mv);
	return *this;
}

Deferred1stPass& Deferred1stPass::setTexture(Texture2D& texture)
{
	texture.bind(m_TexLayer);
	return *this;
}
