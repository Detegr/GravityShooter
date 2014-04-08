layout (location=0) in vec4 vertex;
uniform mat4 MV, P;

void main()
{
	gl_Position = P * MV * vertex;
}
