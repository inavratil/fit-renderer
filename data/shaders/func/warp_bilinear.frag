
////////////////////////////////////////////////////////////////////////////////
//-- Bilinear warping - fragment shader   

#define POLY_OFFSET 100.0

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
	//-- Bilinear warping

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

	f_values.x = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(0,0) ).r;
	f_values.y = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(1,0) ).r;
	f_values.z = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(0,1) ).r;
	f_values.w = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(1,1) ).r;

	M = mat2( f_values.xy, f_values.zw);

	temp = X * M;
	delta.x = dot(temp, Y) * near_far_bias.z;

	//-- diff Y
	f_values.x = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(0,0) ).g;
	f_values.y = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(1,0) ).g;
	f_values.z = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(0,1) ).g;
	f_values.w = textureOffset( MTEX_2Dfunc_values, grid_coords/grid_res + 0.5/grid_res, ivec2(1,1) ).g;

	M = mat2(f_values.xy, f_values.zw);

	temp = X * M;
	delta.y = dot(temp, Y) * near_far_bias.z;

	//-- dx a dy se vztahuji k intervalu [0..1]. My to vsak pricitam k souradnicim, ktery je v intervalu [-1..1], tedy 2x vetsim.
	delta.x *= 2.0;
	delta.y *= 2.0;

	//------------------------------------------------------------------------------------

	return delta;
}

////////////////////////////////////////////////////////////////////////////////


vec3 DPCoordsFront()
{ 
    vec4 texCoords;

    texCoords = lightModelView[0] * io_ObjSpacePosition;

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
    texCoords = lightModelView[1] * io_ObjSpacePosition;
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

////////////////////////////////////////////////////////////////////////////////