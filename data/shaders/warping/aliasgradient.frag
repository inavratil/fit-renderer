out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D tex_blurred_error;
uniform vec2 limit;

void main()
{
	vec4 a, ax, ay;
	vec4 res;

    a = texture(tex_blurred_error, fragTexCoord );
	ax = textureOffset(tex_blurred_error, fragTexCoord, ivec2(1,0) );
	ay = textureOffset(tex_blurred_error, fragTexCoord, ivec2(0,1) );
	float dx = a.a - ax.a;
	float dy = a.a - ay.a;
	//float dx = dFdx(a.a);
	//float dy = dFdy(a.a);

	if( a.r > 70.0 )
		res = vec4( 0.0, 0.0, 1.0, 1.0 );
	else
		res = vec4( clamp( dx, -limit.x, limit.x), clamp( dy, -limit.y, limit.y), 0.0, a.a );


	out_FragColor = res;
}
