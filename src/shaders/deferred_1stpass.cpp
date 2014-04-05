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

	uniform_MVP = uniformLocation("MVP");
	uniform_modelTranslation = uniformLocation("modelTranslation");
}

Deferred1stPass& Deferred1stPass::setMVP(const Matrix4& mvp)
{
	setUniform(uniform_MVP, mvp);
	return *this;
}

Deferred1stPass& Deferred1stPass::setTranslationMatrix(const Matrix4& trans)
{
	setUniform(uniform_modelTranslation, trans);
	return *this;
}
