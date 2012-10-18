
const int PCF_SAMPLES = 5;
const float STEP = 0.2;

//Calculate shadow projection with simple blurred shadows (PCF filtering)
vec4 PCFShadow(in sampler2DShadow shadowMap, in vec4 projection, in float intensity)
{
  float rValue = 1.0;
  float shadow = 0.0;

  projection.z -= 0.01;		//to avoid z-fighting
    
  vec3 coordPos = projection.xyz/projection.w;
  if(coordPos.x > 0.01 && coordPos.y > 0.01 && coordPos.x < 0.99 && coordPos.y < 0.99)
  {
    for(int i=0; i<PCF_SAMPLES; i++)
	{
      for(int j=0; j<PCF_SAMPLES; j++)
	  {
	    float val = textureProj(shadowMap, projection + STEP*vec4(i-1.0, j-1.0, 0.0, 0.0) );
		shadow += val + intensity*float(val < 0.99);
	  }
      rValue = shadow/(PCF_SAMPLES*PCF_SAMPLES);
	}
  }
  return vec4(rValue);
}
