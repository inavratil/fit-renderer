
#define MAX_CASCADES 2
in vec4 o_vertex;       //input vertex
uniform vec2 near_far; // near and far plane for cm-cams
const int cascades = 2;   //number of cascades
uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid

uniform float ZOOM[MAX_CASCADES];    //zoom for both sides of paraboloid

#define PCF_SAMPLES 3
#define PCF_STEP 0.001
#define POLY_OFFSET 100.0

vec3 DPCoordsFront(out int cascade)
{
  /*
    float ZOOM[3];
    ZOOM[0] = 1.0;
    ZOOM[1] = 0.5;
    ZOOM[2] = 0.25;
  */
  
    vec4 texCoords;

    texCoords = lightModelView[0] * o_vertex;

    //texCoords.xyz /= texCoords.w;
    float Length = length( texCoords.xyz );

    texCoords.z *= -1.0;
    texCoords.xyz = normalize( texCoords.xyz );

    texCoords.z += 1.0;
    texCoords.x /= texCoords.z;
    texCoords.y /= texCoords.z;

    //determine cascade
    cascade = 0;
    /*
    if(length(texCoords.xy) < 0.5)
      cascade = 1;
    if(length(texCoords.xy) > 0.25)
      cascade = 2;
    */

    texCoords.xy /= ZOOM[cascade];    //zoom-in

    texCoords.z = (Length - near_far.x)/(near_far.y + POLY_OFFSET - near_far.x);
    texCoords.w = 1.0;


    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}

vec3 DPCoordsBack()
{
    vec4 texCoords;
    texCoords = lightModelView[1] * o_vertex;
    //texCoords.xyz /= texCoords.w;
    float Length = length( texCoords.xyz );

    texCoords.z *= -1.0;
    texCoords.xyz = normalize( texCoords.xyz );

    texCoords.z += 1.0;
    texCoords.x /= texCoords.z;
    texCoords.y /= texCoords.z;

    texCoords.z = (Length - near_far.x)/(near_far.y + POLY_OFFSET - near_far.x);
    texCoords.w = 1.0;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}

//Compute shadow using dual-paraboloid projection
float ShadowOMNI(in sampler2DArray shadow_map, in float intensity)
{
    //calculate front and back coordinates
    int cascade = 0;
    vec3 front_coords = DPCoordsFront(cascade);
    vec3 back_coords = DPCoordsBack();

    //calculate split plane position between paraboloids
    float split_plane = vec4(lightModelView[0] * o_vertex).z;

    float depth, mydepth;

    //select depth value from front or back paraboloid texture (with PCF)
    float result = 0.0;
#ifdef USE_PCF
    for(int i=0; i < PCF_SAMPLES; i++)
    {
        for(int j=0; j<PCF_SAMPLES; j++)
        {
          if(split_plane < 0.0)
          {
              depth = texture(shadow_map, vec3(front_coords.xy + vec2((i - 1.0)*PCF_STEP, (j - 1.0)*PCF_STEP), cascade) ).r;
              mydepth = front_coords.z;
          }
          else
          {
              depth = texture(shadow_map, vec3(back_coords.xy + vec2((i - 1.0)*PCF_STEP, (j - 1.0)*PCF_STEP), cascades) ).r;
              mydepth = back_coords.z;
          }

          //compare depths
          if(depth < mydepth)
              result += intensity;
          else
              result += 1.0;
        }
    }
    return result/(PCF_SAMPLES*PCF_SAMPLES);
#else
      if(split_plane < 0.0)
      {
          depth = texture(shadow_map, vec3(front_coords.xy, 0.0) ).r;
          mydepth = front_coords.z;
      }
      else
      {
          depth = texture(shadow_map, vec3(back_coords.xy, 1.0) ).r;
          mydepth = back_coords.z;
      }

      //compare depths
      if(depth < mydepth)
          result += intensity;
      else
          result += 1.0;

      return result;
#endif
}
