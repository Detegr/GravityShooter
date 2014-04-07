in vec3 position;
in vec3 normals;
in vec2 texCoords;

layout (location=0) out vec4 outpos;
layout (location=1) out vec4 outnormal;
layout (location=2) out vec4 outdiffuse;

uniform sampler2D texSampler;

void main()
{
	outpos = vec4(position, 1.0);
	outnormal = vec4(normals, 1.0);
	outdiffuse = texture(texSampler, texCoords);
}
