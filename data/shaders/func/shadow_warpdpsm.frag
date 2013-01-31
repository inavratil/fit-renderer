////////////////////////////////////////////////////////////////////////////////
//-- Warp dpsm - fragment shader

in vec4 o_vertex;       //input vertex
uniform vec3 near_far_bias; // near and far plane for cm-cams

uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid

#define POLY_OFFSET 100.0

////////////////////////////////////////////////////////////////////////////////
//-- Warp dpsm - vertex shader                                                  

//-- Shared uniforms
uniform vec4 range;
uniform float grid_res;

#ifdef POLYNOMIAL_WARP
	//-- Polynomial uniforms
	uniform mat4 coeffsX;
	uniform mat4 coeffsY;

	//-- Bilinear uniforms
	uniform sampler2D funcTex;
#endif

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;



vec2 computeDiff( vec4 _position )
{
	//------------------------------------------------------------------------------------
	//-- Initialization

	vec2 delta = vec2( 0.0 );
	vec2 p = _position.xy;		//-- zde je p v intervalu [-1..1]
	p = p*0.5 + 0.5;			//-- prevod p na interval [0..1]

	//------------------------------------------------------------------------------------
	// Polynomial warping

#ifdef POLYNOMIAL_WARP

	p = p * vec2( SCREEN_X, SCREEN_Y );

	float new_x = (p.x - range.x)/(range.y - range.x) * (3.0 - 0.0) + 0.0;
	float new_y = (p.y - range.z)/(range.w - range.z) * (3.0 - 0.0) + 0.0;

	vec4 X = vec4( 1.0, new_x, pow(new_x, 2.0), pow(new_x,3.0) );
	vec4 Y = vec4( 1.0, new_y, pow(new_y, 2.0), pow(new_y,3.0) );
		
	vec4 temp = X * coeffsX;
	delta.x = dot(temp, Y) * near_far_bias.z;
	temp = X * coeffsY;
	delta.y = dot(temp, Y) * near_far_bias.z;

	//------------------------------------------------------------------------------------
	//-- Bilinear warping

#else ifdef BILINEAR_WARP

	p = p * (grid_res-1); // prevod z [0..1] do [0..res-1]

	//-- vypocet souradnice bunky, ve ktere se bod "p" nachazi
	vec2 grid_coords = floor( p.xy );
	
	vec2 temp, X, Y;
	mat2 M;
	vec4 f_values;

	//-- prevod do intervalu [0..1]
	float x = fract(p.x);
	float y = fract(p.y);

	X = vec2(1-x,x);
	Y = vec2(1-y,y);

	//-- diff X
	//FIXME: Nefunguje!?!? vec4 t = textureGather( funcTex, vec2(0,0));

	f_values.x = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,0) ).r;
	f_values.y = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,0) ).r;
	f_values.z = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,1) ).r;
	f_values.w = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,1) ).r;

	M = mat2( f_values.xy, f_values.zw);

	temp = X * M;
	delta.x = dot(temp, Y) * near_far_bias.z;

	//-- diff Y
	f_values.x = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,0) ).g;
	f_values.y = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,0) ).g;
	f_values.z = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,1) ).g;
	f_values.w = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,1) ).g;

	M = mat2(f_values.xy, f_values.zw);

	temp = X * M;
	delta.y = dot(temp, Y) * near_far_bias.z;

	//-- dx a dy se vztahuji k intervalu [0..1]. My to vsak pricitam k souradnicim, ktery je v intervalu [-1..1], tedy 2x vetsim.
	delta.x *= 2.0;
	delta.y *= 2.0;

	//------------------------------------------------------------------------------------

#endif
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

    texCoords.z = (Length - near_far_bias.x)/(near_far_bias.y + POLY_OFFSET - near_far_bias.x);
    texCoords.w = 1.0;

	vec2 d = computeDiff( texCoords );
	texCoords.xy += d;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}

vec3 DPCoordsBack(out vec2 d)
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

    texCoords.z = (Length - near_far_bias.x)/(near_far_bias.y + POLY_OFFSET - near_far_bias.x);
    texCoords.w = 1.0;

	d = computeDiff( texCoords );
	texCoords.xy += d;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}

//Compute shadow using dual-paraboloid projection
float ShadowOMNI(in sampler2DArray shadow_map, in float intensity)
{
    //calculate front and back coordinates
	vec2 delta_back;
    vec3 front_coords = DPCoordsFront();
    vec3 back_coords = DPCoordsBack(delta_back);

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

      return result; // vec4( delta_back, floor(back_coords.xy * 1024.0) );

}

////////////////////////////////////////////////////////////////////////////////