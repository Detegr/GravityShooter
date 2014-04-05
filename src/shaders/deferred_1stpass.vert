layout (location=0) in vec4 vertex;
layout (location=2) in vec3 normal;
uniform mat4 MV, P;
out vec3 position;
out vec3 normals;

void main()
{
	gl_Position = P * MV * vertex;
	position = (MV * vertex).xyz;
	normals = normalize((MV * vec4(normal, 0.0)).xyz);
}
