uniform sampler2D mat_aliasError;

//uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid
//uniform vec3 near_far_bias; // near and far plane for cm-cams
//uniform vec4 range;

//mipmap level from which we read
uniform int mip_level;
uniform float offset;

in vec2 fragTexCoord;
out vec4 out_FragColor;

void main()
{
	vec4 out_color = vec4( 0.0 );
	// vyraz "-vec2(offset)", protoze souradnice z levelu i+1 cte z pixel o souradnicich (1,1) v levelu i - brano v ramci 2x2 pixelu 
	float errorVal00 = textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(0,0) ).a;
	float errorVal10 = textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(1,0) ).a;
	float errorVal01 = textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(0,1) ).a;
	float errorVal11 = textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(1,1) ).a;
	
	float maxWidth = max(errorVal00 + errorVal10, errorVal01 + errorVal11 );
	float maxHeight = max(errorVal00 + errorVal01, errorVal10 + errorVal11 );

	float maxVal = max(maxWidth, maxHeight);

	//zde musim ukladat vzdy do aplha kanalu. Z toho se pak cte o uroven niz
	out_color.r = sqrt(errorVal00 + errorVal10 + errorVal01 + errorVal11);
	out_color.a = errorVal00 + errorVal10 + errorVal01 + errorVal11;
	out_FragColor = out_color;
	//out_FragColor = vec4( maxVal );
}