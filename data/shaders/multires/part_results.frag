out vec4 out_FragColor;
in vec2 fragTexCoord;

//textures
uniform sampler2D MTEX_norm_lindepth, MTEX_eyepos, MTEX_color, MTEX_derivs, MTEX_minmax_norm, MTEX_depth_stencil;

uniform int show_mip = 0;

void main()
{
	out_FragColor = texture(MTEX_depth_stencil, fragTexCoord);
	//out_FragColor = textureLod(MTEX_minmax_norm, fragTexCoord, show_mip);
}
 