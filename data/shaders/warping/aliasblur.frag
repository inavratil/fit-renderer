// Author: Juraj Vanek
// This shader performs vertical blur
out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D bloom_texture;
uniform float kernel_size; //-- musi byt liche cislo

//Gaussian filter constant
uniform float two_sigma_sq;  // = 2*sigma^2
uniform float frac_sqrt_two_sigma_sq;  // = 1.0/sqrt(2*PI*sigma^2)

uniform ivec2 texsize;

void main()
{
	float r = texture(bloom_texture, fragTexCoord).r;
	//if( r > 50.0 ) discard;

    vec4 result = vec4( 0.0 );
    float gauss_func, div_factor = 0.0;
    float k_shift;

    vec2 coord = fragTexCoord;
    vec2 disp_coord;

    //Gauss filter
    int i;
    for(i = 0; i < kernel_size; i++)
    {
#ifdef HORIZONTAL
      k_shift = float(i - (kernel_size-1.0)/2) * (1.0/float(texsize.x));
      disp_coord = coord + vec2(k_shift, 0.0);
#else
      k_shift = float(i - (kernel_size-1.0)/2) * (1.0/float(texsize.y));
      disp_coord = coord + vec2(0.0, k_shift);
#endif
	  vec4 c = texture(bloom_texture, disp_coord);
	  if( c.r > 50.0 ) c = vec4(0.0);

      gauss_func = frac_sqrt_two_sigma_sq*exp(-(abs(k_shift)*abs(k_shift))/two_sigma_sq);
      div_factor += gauss_func;

      result += gauss_func * c;
    }
    out_FragColor = result/div_factor;
}
