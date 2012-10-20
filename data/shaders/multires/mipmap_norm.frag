//create mipmaps from min/max normals, output maximum value into next finer level
uniform sampler2D MTEX_minmax_norm;

in vec2 fragTexCoord;
out vec4 out_FragColor;

//mipmap level from which we read
uniform int mip_level;

void main(void)
{
	vec2 minNorm, maxNorm;
	
	vec4 texColor0 = textureLod( MTEX_minmax_norm, fragTexCoord, mip_level );
	minNorm = texColor0.xy;
	maxNorm = texColor0.zw;

	vec4 texColor1 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(1,0) );
	minNorm = min( minNorm, texColor1.xy );
	maxNorm = max( maxNorm, texColor1.zw );
	
	vec4 texColor2 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(1,1) );
	minNorm = min( minNorm, texColor2.xy );
	maxNorm = max( maxNorm, texColor2.zw );
	
	vec4 texColor3 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(0,1) );
	minNorm = min( minNorm, texColor3.xy );
	maxNorm = max( maxNorm, texColor3.zw );
	
	vec4 texColor4 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(-1,0) );
	minNorm = min( minNorm, texColor4.xy );
	maxNorm = max( maxNorm, texColor4.zw );
	
	vec4 texColor5 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(0,-1) );
	minNorm = min( minNorm, texColor5.xy );
	maxNorm = max( maxNorm, texColor5.zw );
	
	vec4 texColor6 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(-1,-1) );
	minNorm = min( minNorm, texColor6.xy );
	maxNorm = max( maxNorm, texColor6.zw );
	
	vec4 texColor7 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(-1,1) );
	minNorm = min( minNorm, texColor7.xy );
	maxNorm = max( maxNorm, texColor7.zw );
	
	vec4 texColor8 = textureLodOffset( MTEX_minmax_norm, fragTexCoord, mip_level, ivec2(1,-1) );
	minNorm = min( minNorm, texColor8.xy );
	maxNorm = max( maxNorm, texColor8.zw );

	out_FragColor = vec4( minNorm, maxNorm );
}