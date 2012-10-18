
//material settings
struct Material{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
uniform Material material;


//shared Light uniform block
layout(std140) uniform Lights{
    vec3 position[LIGHTS];
    vec3 ambient[LIGHTS];
    vec3 diffuse[LIGHTS];
    vec3 specular[LIGHTS];
    float radius[LIGHTS];
    //vec3 camPos;
}lights;

//Calculate light model
vec4 LightModel(in vec3 normal, in vec3 eyeVec)
{
  vec3 final_color = vec3(0.0);
  vec3 N = normalize(normal);
  vec3 E = normalize(eyeVec);
  vec3 lightDir, L;
  float lambertTerm, specular, distsqr, att;

  for(int i=0; i<LIGHTS; i++)
  {
    final_color += lights.ambient[i] * material.ambient;
    lightDir = (lights.position[i] + eyeVec)/lights.radius[i];
    att = max(0.0, 1.0 - dot(lightDir, lightDir));
    L = normalize(lightDir);

  	lambertTerm = dot(N,L);
  	if(lambertTerm > 0.0)
  	{
  		final_color += lights.diffuse[i] * material.diffuse * lambertTerm * att;
  		vec3 R = reflect(-L, N);
  		specular = pow( max(dot(R, E), 0.0), material.shininess );
  		final_color += lights.specular[i].rgb *  material.specular * specular * att;
  	}
  }

  return vec4(final_color,0.0);
}

