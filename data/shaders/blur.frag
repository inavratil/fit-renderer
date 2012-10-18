// Author: Juraj Vanek
// This shader performs vertical blur
out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D bloom_texture;
const int kernel_size = 15;

//Gaussian filter constant
const float sigma = 25.132741;  // = 2*PI*sigma^2
const float frac_sqrt_sigma = 0.1994711;  // = 1.0/sqrt(2*PI*sigma^2)

uniform ivec2 texsize;

void main()
{
    vec4 result = vec4(0.0);
    float gauss_func, div_factor = 0.0;
    float k_shift;

    vec2 coord = fragTexCoord/2.0;
    vec2 disp_coord;

    //Gauss filter
    int i;
    for(i = 0; i < kernel_size; i++)
    {
#ifdef HORIZONTAL
      k_shift = float(i - kernel_size/2) * (1.0/float(texsize.x));
      disp_coord = coord + vec2(k_shift, 0.0);
#else
      k_shift = float(i - kernel_size/2) * (1.0/float(texsize.y));
      disp_coord = coord + vec2(0.0, k_shift);
#endif
      gauss_func = frac_sqrt_sigma*exp(-(abs(k_shift)*abs(k_shift))/sigma);
      div_factor += gauss_func;

      result += gauss_func * texture(bloom_texture, disp_coord);
    }
    out_FragColor = result/div_factor;
}
