////////////////////////////////////////////////////////////////////////////////
//-- Warp dpsm - fragment shader

in vec4 o_vertex;       //input vertex
uniform vec2 near_far; // near and far plane for cm-cams

uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid

#define POLY_OFFSET 100.0

////////////////////////////////////////////////////////////////////////////////
//-- Warp dpsm - vertex shader                                                  

uniform mat4 coeffsX;
uniform mat4 coeffsY;

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;

const int INSIDE = 0;	//0x0000;
const int TOP = 1;		//0x0001;
const int BOTTOM = 2;	//0x0010;
const int RIGHT = 4;	//0x0100;
const int LEFT = 8;		//0x1000;

int computeCode(vec2 _p, vec4 range)
{
        int ret;
 
        ret = INSIDE;       
 
        if (_p.x < range.x)           // to the left of clip window
                ret |= LEFT;
        else if (_p.x > range.y)      // to the right of clip window
                ret |= RIGHT;
        if (_p.y < range.z)           // below the clip window
                ret |= BOTTOM;
        else if (_p.y > range.w)      // above the clip window
                ret |= TOP;
 
        return ret;
}

//------------------------------------------------------------------------------------
// Polynomial warping

vec2 computeDiff( vec4 _position )
{
	vec4 range = vec4( 41.0, 83.0, 11.0, 41.0 );

	vec2 p = _position.xy;
	p = p*0.5 + 0.5;
	p = p * vec2( SCREEN_X, SCREEN_Y );

	int code = computeCode(p, range);

	vec2 delta = vec2( 0.0 );
	if ( code == INSIDE )
	{
		float new_x = (p.x - range.x)/(range.y - range.x) * (3.0 - 0.0) + 0.0;
		float new_y = (p.y - range.z)/(range.w - range.z) * (3.0 - 0.0) + 0.0;

		vec4 X = vec4( 1.0, new_x, pow(new_x, 2.0), pow(new_x,3.0) );
		vec4 Y = vec4( 1.0, new_y, pow(new_y, 2.0), pow(new_y,3.0) );
		
		vec4 temp = X * coeffsX;
		delta.x = dot(temp, Y) * 25.0/1024.0;
		temp = X * coeffsY;
		delta.y = dot(temp, Y) * 25.0/1024.0;
		//dx = dx * 2.0 - 1.0;
		//dy = dy * 2.0 - 1.0
	}

	return delta;
}

////////////////////////////////////////////////////////////////////////////////


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

	vec2 d = computeDiff( texCoords );
	texCoords.xy += d;

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

	vec2 d = computeDiff( texCoords );
	texCoords.xy += d;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}

//Compute shadow using dual-paraboloid projection
vec4 ShadowOMNI(in sampler2DArray shadow_map, in float intensity)
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
      if(depth < mydepth)
          result += intensity;
      else
          result += 1.0;

      return vec4( depth, mydepth, floor(back_coords.xy * 1024.0) );

}

////////////////////////////////////////////////////////////////////////////////