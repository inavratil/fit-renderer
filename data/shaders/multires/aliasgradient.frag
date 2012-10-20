out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D tex;

void main()
{
	vec4 a;
	vec4 res;

    a = texture(tex, fragTexCoord );
	float dx = dFdx(a.a);
	float dy = dFdy(a.a);

	if( a.r > 50.0 )
		res = vec4( 0.0, 0.0, 1.0, 1.0 );
	else
		res = vec4( clamp( dx, -1.0, 1.0), clamp( dy, -1.0, 1.0), 0.0, 99.0);


	out_FragColor = res;
}
