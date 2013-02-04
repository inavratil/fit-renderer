// @INPUT: io_ObjSpacePosition

//Compute shadow using dual-paraboloid projection
float ShadowOMNI(in sampler2DArray shadow_map, in float intensity)
{
    //calculate front and back coordinates
	vec2 delta_back;
    vec3 front_coords = DPCoordsFront();
    vec3 back_coords = DPCoordsBack(delta_back);

    //calculate split plane position between paraboloids
    float split_plane = vec4(lightModelView[0] * io_ObjSpacePosition).z;

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
      if(depth < mydepth)
          result += intensity;
      else
          result += 1.0;

      return result; // vec4( delta_back, floor(back_coords.xy * 1024.0) );

}
