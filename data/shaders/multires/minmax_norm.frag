//Calculate min/max normal values
uniform sampler2D MTEX_norm_lindepth;	//normals + depth values

in vec2 fragTexCoord;
out vec4 out_FragColor;

void main(void)
{
	vec4 fragNorm = texture( MTEX_norm_lindepth, fragTexCoord );

	vec2 norm0 = fragNorm.xy;
	vec2 norm1 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(1,0) ).xy;
	vec2 maxNorm = max(norm0,norm1);
	vec2 minNorm = min(norm0,norm1);

	vec2 norm2 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(0,1) ).xy;
	maxNorm = max(maxNorm,norm2);
	minNorm = min(minNorm,norm2);

	vec2 norm3 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(1,1) ).xy;
	maxNorm = max(maxNorm,norm3);
	minNorm = min(minNorm,norm3);

	vec2 norm4 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(-1,0) ).xy;
	maxNorm = max(maxNorm,norm4);
	minNorm = min(minNorm,norm4);

	vec2 norm5 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(0,-1) ).xy;
	maxNorm = max(maxNorm,norm5);
	minNorm = min(minNorm,norm5);

	vec2 norm6 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(-1,-1) ).xy;
	maxNorm = max(maxNorm,norm6);
	minNorm = min(minNorm,norm6);

	vec2 norm7 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(-1,1) ).xy;
	maxNorm = max(maxNorm,norm7);
	minNorm = min(minNorm,norm7);

	vec2 norm8 = textureOffset( MTEX_norm_lindepth, fragTexCoord, ivec2(1,-1) ).xy;
	maxNorm = max(maxNorm,norm8);
	minNorm = min(minNorm,norm8);
	
	//depth derivative
	vec4 minMaxNorms = vec4( minNorm, maxNorm );
	
	out_FragColor = minMaxNorms; 
}