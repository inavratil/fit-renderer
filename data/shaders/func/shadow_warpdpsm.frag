////////////////////////////////////////////////////////////////////////////////
//-- Warp dpsm - fragment shader

in vec4 o_vertex;       //input vertex
uniform vec2 near_far; // near and far plane for cm-cams

uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid

#define POLY_OFFSET 100.0

vec3 DPCoordsFront()
{ 
    vec4 texCoords;

    texCoords = lightModelView[0] * o_vertex;

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
    vec3 front_coords = DPCoordsFront();
    vec3 back_coords = DPCoordsBack();

    //calculate split plane position between paraboloids
    float split_plane = vec4(lightModelView[0] * o_vertex).z;

    float depth, mydepth;

    //select depth value from front or back paraboloid texture (with PCF)
    float result = 0.0;

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
      //if(depth < mydepth)
	  if(split_plane < 0.0)
          result += 0.0; //intensity;
      else
          result += 1.0;

      return result;

}

////////////////////////////////////////////////////////////////////////////////