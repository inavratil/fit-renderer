uniform sampler2D tex_error_color;
uniform sampler2D tex_2Dfunc_values;

uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid
uniform vec3 near_far_bias; // near and far plane for cm-cams
uniform vec4 range;
uniform mat4 in_ModelViewMatrix;

in  vec4 o_vertex;
out vec4 out_fragColor;

#define POLY_OFFSET 100.0

const float SM_RES = 128.0;
const float TWO_TAN_TH = 0.828427; // FOV = 45'
////////////////////////////////////////////////////////////////////////////////
//-- Bilinear warping - fragment shader   

uniform float grid_res;

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

	output.x = textureOffset( tex_2Dfunc_values, grid_pos, ivec2(_x,0) )[comp];
	output.y = textureOffset( tex_2Dfunc_values, grid_pos, ivec2(_x,1) )[comp];
	output.z = textureOffset( tex_2Dfunc_values, grid_pos, ivec2(_x,2) )[comp];
	output.w = textureOffset( tex_2Dfunc_values, grid_pos, ivec2(_x,3) )[comp];

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
	delta.x = dot(Q * Mcr, Ty) * near_far_bias.z;

	//-- diff Y

	P = mat4(
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 0, 1 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 1, 1 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 2, 1 ),
		GetPoints( grid_coords/grid_res + 0.5/grid_res, 3, 1 )
		);
	
	Q = P * Mcr * Tx;
	delta.y = dot(Q * Mcr, Ty) * near_far_bias.z;	

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

    texCoords.z = (Length - near_far_bias.x)/(near_far_bias.y + POLY_OFFSET - near_far_bias.x);
    texCoords.w = 1.0;

	vec2 d = computeDiff( texCoords );
	texCoords.xy += d;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}
void main(void)
{
    vec3 front_coords = DPCoordsFront();
    vec3 back_coords = DPCoordsBack();

	vec4 vertexEyeSpace = in_ModelViewMatrix * o_vertex;
    vec4 vertexLightSpace = lightModelView[0] * o_vertex;
    float split_plane = -vertexLightSpace.z;

    vec4 color_result = vec4( 0.0 );
	
	float wi = (TWO_TAN_TH/SCREEN_X)*( -vertexEyeSpace.z ); //-- minus, protoze osa Z smeruje za kameru
	color_result = vec4( wi );
#if 0
    vec3 curr_texCoords;
    if(split_plane >= 0.0)
        curr_texCoords = front_coords;
    else
        curr_texCoords = back_coords;
    
	vec4 zoomed_range = range / 128.0;					//-- prevod do [0..1]
	//curr_texCoords.xy = (curr_texCoords.xy - zoomed_range.xz) / (zoomed_range.yw - zoomed_range.xz);
	//-- aliasing error

    vec2 ds, dt;
    {
        ds = dFdx(curr_texCoords.xy) * SM_RES;
        dt = dFdy(curr_texCoords.xy) * SM_RES;
    }

    mat2 ma = mat2( ds, dt );
    float ma_error = 1.0/determinant( ma );
    float md_error = 1.0/max( length(ds+dt), length(ds-dt) );

    float res_error = clamp(md_error, 1.0/11.0, 11.0);

    if( res_error < 1.0 )

        res_error = (res_error - 1.0/11.0) / (1.0 - 1.0/11.0) * 0.5; //-- from [1/11..11] to [0..1]
    else
        res_error = (res_error - 1.0) / (11.0 - 1.0) * (1.0 - 0.5) + 0.5; //-- from [1/11..11] to [0..1]

    vec3 c = texture( tex_error_color, vec2(res_error,0.0) ).xyz;
    color_result = vec4( c, 1.0 );

    //-- grid   
#if 0
    float gridLineWidth = 1.0;

    vec2 scaledTC = curr_texCoords.xy / (20.0 * 1.0/1024.0);
    vec2 dS = vec2(dFdx(scaledTC.s),dFdy(scaledTC.s));
    vec2 dT = vec2(dFdx(scaledTC.t),dFdy(scaledTC.t));
    vec2 m = fract( scaledTC );
    if( m.s < gridLineWidth*length(dS) || m.t < gridLineWidth*length(dT) )
        color_result = vec4( 1.0, 0.0, 0.0, 1.0 );
#endif
    //-- End

#endif
    out_fragColor = color_result;
    out_fragColor.a = 1.0;// md_error;
}
