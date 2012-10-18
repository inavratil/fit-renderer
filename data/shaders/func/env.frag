
//Calculate color from environment map
//Insipred by: http://www.ozone3d.net/tutorials/glsl_texturing_p04.php
vec4 envMapping(in vec4 base, in vec3 eye, in vec3 normal, in sampler2D envMap, in float intensity)
{
  const vec3 Xunitvec = vec3 (1.0, 0.0, 0.0);
  const vec3 Yunitvec = vec3 (0.0, 1.0, 0.0);

  //compute reflection vector
  vec3 reflectDir = reflect(eye, normal);
    
  //compute altitude and azimuth angles
  vec2 index;
  index.y = dot(normalize(reflectDir), Yunitvec);
  reflectDir.y = 0.0;
  index.x = dot(normalize(reflectDir), Xunitvec) * 0.5;

  //translate index values into proper range
  if (reflectDir.z >= 0.0)
    index = (index + 1.0) * 0.5;
  else
  {
    index.t = (index.t + 1.0) * 0.5;
    index.s = (-index.s) * 0.5 + 1.0;
  }
  //do a lookup into the environment map.
  vec4 envColor = texture(envMap, index);
  envColor = mix(envColor, base, intensity);
  return envColor;
}
