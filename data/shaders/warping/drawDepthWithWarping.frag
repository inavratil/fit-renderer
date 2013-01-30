in vec2 fragTexCoord;
in vec4 position;
in vec4 res;

out vec4 frag_Color;

void main()
{
	gl_FragDepth = position.z;
	
	frag_Color = vec4( res );
	//frag_Color.a = 1.0;
}
