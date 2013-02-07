
////////////////////////////////////////////////////////////////////////////////
//-- Bilinear warping - fragment shader   

#define POLY_OFFSET 100.0

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;

const mat4 Mcr = 0.5 * mat4(
		 0.0f,	2.0f,	0.0f,	0.0f,
		-1.0f,	0.0f,	1.0f,	0.0f,
		 2.0f, -5.0f,	4.0f,  -1.0f,
		-1.0f,	3.0f,	-3.0f,  1.0f
		);

vec4 GetPoints( in vec2 grid_pos, in int _x, in int comp )
{
	vec4 output;

	output.x = textureOffset( MTEX_2Dfunc_values, grid_pos, ivec2(_x,0) )[comp];
	output.y = textureOffset( MTEX_2Dfunc_values, grid_pos, ivec2(_x,1) )[comp];
	output.z = textureOffset( MTEX_2Dfunc_values, grid_pos, ivec2(_x,2) )[comp];
	output.w = textureOffset( MTEX_2Dfunc_values, grid_pos, ivec2(_x,3) )[comp];

	return output;
}

vec2 computeDiff( vec4 _position )
{
	//------------------------------------------------------------------------------------
	//-- Initialization

	vec2 delta = vec2( 0.0 );
	vec2 p = _position.xy;		//-- zde je p v intervalu [-1..1]
	p = p*0.5 + 0.5;			//-- prevod p na interval [0..1]

	//------------------------------------------------------------------------------------
	//-- Spline warping

	//FIXME: grid_res je mrizka i s rozsirenim na kraji, ale ja potrebuji tu skutecnou - 1
	p = p * (grid_res -2.0-1); // prevod z [0..1] do [0..res-1]

	//-- vypocet souradnice bunky, ve ktere se bod "p" nachazi
	vec2 grid_coords = floor( p.xy );
	
	mat4 P;
	vec4 Tx, Ty;
	vec4 Q;

	//-- prevod do intervalu [0..1]
	float x = fract(p.x);
	float y = fract(p.y);

	Tx = vec4( 1.0, x, pow(x, 2.0f), pow(x,3.0f) );
	Ty = vec4( 1.0, y, pow(y, 2.0f), pow(y, 3.0f) );

	//-- diff X
	//FIXME: Nefunguje!?!? vec4 t = textureGather( funcTex, vec2(0,0));
	
	P = mat4(
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 0, 0 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 1, 0 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 2, 0 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 3, 0 )
		);
	
	Q = P * Mcr * Tx;
	delta.x = dot(Q * Mcr, Ty);

	//-- diff Y

	P = mat4(
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 0, 1 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 1, 1 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 2, 1 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 3, 1 )
		);
	
	Q = P * Mcr * Tx;
	delta.y = dot(Q * Mcr, Ty);	

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