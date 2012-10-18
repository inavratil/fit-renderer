// Author: Juraj Vanek
// This shader handles extraction of bright parts of the scene and computation of SSAO effect

//texture samplers
in vec2 fragTexCoord;
uniform sampler2D mat_bloomBaseA, normal_texture, render_texture;

//output color
out vec4 out_FragColor;

#ifdef BLOOM
  //Bloom variables
  uniform float THRESHOLD;
#endif

#ifdef SSAO
const float FAR = 1000.0;   //distance of far plane
const int iterations = 16;

uniform float radius = 0.005;	//???
uniform float bias = 1.5;
uniform float intensity = 1.6;

float ssao(vec2 uv)
{
  const vec3 vec[16] = vec3[](vec3(1,0,0),vec3(-1,0,0), vec3(0,1,0), vec3(0,-1,0), vec3(1,1,0),vec3(-1,1,0), vec3(1,-1,0), vec3(-1,-1,0),
                              vec3(1,0,1),vec3(-1,0,1), vec3(0,1,1), vec3(0,-1,1), vec3(1,1,1),vec3(-1,1,1), vec3(1,-1,0), vec3(-1,-1,1));

  vec4 curr_sample = texture(normal_texture, uv);
  float curr_depth = curr_sample.a;
  vec3 rand = texture(mat_bloomBaseA, 50.0*uv).xyz;

  float ao = 0.0f;

  //**SSAO Calculation**//
  float rad = 1.0/(curr_depth/FAR);
  
  for (int i = 0; i < iterations; ++i)
  {
    vec3 ray = vec3(reflect(vec[i], rand)*rad);   //reflect ray into hemisphere
    ray *= sign(dot(ray,curr_sample.xyz));    //restrict rays into hemisphere

    vec4 n_sample = texture(normal_texture, uv + radius*ray.xy, 0);
    float depth_diff = distance(vec3(0.0, 0.0, curr_depth), vec3(ray.xy, n_sample.a))/FAR;

    if(depth_diff > 1.0)  //solve problem with far skybox and false occlusion
      ao += 1.0;
    else
      ao += max(0.0,dot(n_sample.xyz, curr_sample.xyz))*(bias/(1.0+min(1.0,depth_diff)));
  }
  ao /= iterations;
  return min(1.0,pow(ao, intensity));
}

#endif

//////
//MAIN
void main()
{
  vec3 result = vec3(0.0);
  float ssao_term = 1.0;
  int i,j;

#ifdef BLOOM
  //fetch color texture
  vec4 texel = texture(render_texture, fragTexCoord);
  //extract brightest parts of image
  if( dot(vec3(0.30, 0.59, 0.11),texel.rgb) > THRESHOLD)
    result = texel.rgb;
#endif

#ifdef SSAO
  //compute SSAO term
  ssao_term = ssao(fragTexCoord);
#endif

  out_FragColor.rgb = result;
  out_FragColor.a = ssao_term;
}
