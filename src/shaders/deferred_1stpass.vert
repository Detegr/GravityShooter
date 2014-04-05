layout (location=0) in vec4 vertex;
layout (location=2) in vec3 normal;
uniform mat4 MVP;
uniform mat4 modelTranslation;
out vec3 position;
out vec3 normals;

void main()
{
	gl_Position = MVP * vertex;
	position = (modelTranslation * vertex).xyz;
	normals = normalize((modelTranslation * vec4(normal, 0.0)).xyz);
}
