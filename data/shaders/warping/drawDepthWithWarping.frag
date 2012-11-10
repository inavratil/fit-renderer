in vec2 fragTexCoord;
in vec4 position;
in vec4 res;

out vec4 frag_Color;

void main()
{
	gl_FragDepth = position.z;
	vec4 pos = res*0.5 + 0.5;

	frag_Color = vec4( 10.0 * position.z );
	frag_Color.a = 1.0;
}
