//store max. derivative of input depth
uniform sampler2D MTEX_norm_lindepth;		//normals and linear depth

/*uniform*/ const float far_plane = 1000.0;

in vec2 fragTexCoord;
out vec4 out_FragColor;

void main(void)
{
	float depth0 = texture(MTEX_norm_lindepth, fragTexCoord).w;
	float depth1 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(1,0)).w;
	float maxDepth = max(depth0,depth1);
	float minDepth = min(depth0,depth1);

	float depth2 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(0,1)).w;
	maxDepth = max(maxDepth,depth2);
	minDepth = min(minDepth,depth2);

	float depth3 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(1,1)).w;
	maxDepth = max(maxDepth,depth3);
	minDepth = min(minDepth,depth3);

	float depth4 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(-1,0)).w;
	maxDepth = max(maxDepth,depth4);
	minDepth = min(minDepth,depth4);

	float depth5 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(0,-1)).w;
	maxDepth = max(maxDepth,depth5);
	minDepth = min(minDepth,depth5);

	float depth6 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(-1,-1)).w;
	maxDepth = max(maxDepth,depth6);
	minDepth = min(minDepth,depth6);

	float depth7 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(1,-1)).w;
	maxDepth = max(maxDepth,depth7);
	minDepth = min(minDepth,depth7);

	float depth8 = textureOffset(MTEX_norm_lindepth, fragTexCoord, ivec2(-1,1)).w;
	maxDepth = max(maxDepth,depth8);
	minDepth = min(minDepth,depth8);
	
	//the derivative of the local linear depth
	out_FragColor = far_plane*vec4(maxDepth-minDepth)/depth0;
}