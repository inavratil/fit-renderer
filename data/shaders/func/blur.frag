
//Performs simple blur
#define BLUR_STRENGTH 2
vec4 Blur(in sampler2D tex, vec2 texCoord, in float step)
{
  vec3 result = vec3(0.0);
  
  for(int i=0; i<BLUR_STRENGTH; i++)
    for(int j=0; j<BLUR_STRENGTH; j++)
      result += texture(tex,vec2( texCoord.x+(i-1.0)*step, texCoord.y+(j-1.0)*step)).xyz;

  return vec4(result/(BLUR_STRENGTH), 0.0);
}
