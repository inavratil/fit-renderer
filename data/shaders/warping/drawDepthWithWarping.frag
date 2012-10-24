in vec2 fragTexCoord;
in vec4 position;
in vec4 res;

void main()
{
	gl_FragDepth = position.z;
}
