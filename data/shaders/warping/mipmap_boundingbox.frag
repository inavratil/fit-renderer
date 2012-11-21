//create mipmaps from derivatives, output maximum value into next finer level
uniform sampler2D MTEX_mask;

in vec2 fragTexCoord;
out vec4 out_FragColor;

//mipmap level from which we read
uniform int mip_level;

void main(void)
{
	float maxVal = textureLod( MTEX_mask, fragTexCoord, mip_level).r;
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(1,0) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(1,1) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(0,1) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(-1,0) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(0,-1) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(-1,-1) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(1,-1) ).r,maxVal);
	maxVal = max(textureLodOffset( MTEX_mask, fragTexCoord, mip_level, ivec2(-1,1) ).r,maxVal);
		
	out_FragColor = vec4(maxVal);
}