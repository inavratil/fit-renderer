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
	// vyraz "-vec2(offset)", protoze souradnice z levelu i+1 cte z pixel o souradnicich (1,1) v levelu i - brano v ramci 2x2 pixelu 
	float errorVal00 = pow( textureLod( mat_aliasError, fragTexCoord-vec2(offset), mip_level ).a, 2.0);
	float errorVal10 = pow( textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(1,0) ).a, 2.0);
	float errorVal01 = pow( textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(0,1) ).a, 2.0);
	float errorVal11 = pow( textureLodOffset( mat_aliasError, fragTexCoord-vec2(offset), mip_level, ivec2(1,1) ).a, 2.0);
	
	float maxWidth = max(errorVal00 + errorVal10, errorVal01 + errorVal11 );
	float maxHeight = max(errorVal00 + errorVal01, errorVal10 + errorVal11 );

	float maxVal = max(maxWidth, maxHeight);
	out_FragColor = vec4( sqrt(errorVal00 + errorVal10 + errorVal01 + errorVal11) );
	//out_FragColor = vec4( maxVal );
}