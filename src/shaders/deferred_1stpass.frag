in vec3 position;
in vec3 normals;
out vec4 color;

void main()
{
	color.rgb = position;
	color.a = 1.0;
}
