uniform sampler2D mat_aliasError;

//uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid
//uniform vec3 near_far_bias; // near and far plane for cm-cams
//uniform vec4 range;

//mipmap level from which we read
uniform int mip_level;

in vec2 fragTexCoord;
out vec4 out_FragColor;

void main()
{
	vec4 texColor0 = textureLod( mat_aliasError, fragTexCoord, mip_level );
	
	//out_FragColor = vec4( texColor0.a + texColor1.a + texColor2.a + texColor3.a );
	out_FragColor = vec4( texColor0.a*4.0 );
}