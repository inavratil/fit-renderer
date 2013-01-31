//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;

uniform mat4 in_ModelViewMatrix;
uniform vec3 near_far_bias; // near and far plane for cm-cams

out vec2 fragTexCoord;
out vec4 position;
out vec4 res;

//-- Shared uniforms
uniform vec4 range;
uniform float grid_res;

#ifdef POLYNOMIAL_WARP
	//-- Polynomial uniforms
	uniform mat4 coeffsX;
	uniform mat4 coeffsY;
#else ifdef BILINEAR_WARP
	//-- Bilinear uniforms
	uniform sampler2D funcTex;
#endif

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;

void main(void)
{
    fragTexCoord = in_Coord.xy;

    vec4 vertexEyeSpace = in_ModelViewMatrix * vec4(in_Vertex,1.0);
    gl_ClipDistance[0] = -vertexEyeSpace.z;
    
    float Length = length( vertexEyeSpace.xyz );
    vertexEyeSpace.z *= -1.0;
    vertexEyeSpace.xyz = normalize( vertexEyeSpace.xyz );

    vertexEyeSpace.z += 1.0;
    vertexEyeSpace.xy /= vertexEyeSpace.z;

    vertexEyeSpace.z = (Length - near_far_bias.x)/(near_far_bias.y  - near_far_bias.x);
    vertexEyeSpace.w = 1.0; 

	position = vertexEyeSpace;

	//------------------------------------------------------------------------------------
	//-- Initialization
		
	float dx,dy;
	res = vec4( 0.0 );

	vec2 p = position.xy;				//-- zde je p v intervalu [-1..1]
	p = p*0.5 + 0.5;					//-- prevod p na interval [0..1]
	

	//------------------------------------------------------------------------------------
	//-- Polynomial warping

#ifdef POLYNOMIAL_WARP

	p = p * vec2( SCREEN_X, SCREEN_Y );	//-- p nabyva hodnot [0..128]

	float new_x = (p.x - range.x)/(range.y - range.x) * (3.0 - 0.0) + 0.0;
	float new_y = (p.y - range.z)/(range.w - range.z) * (3.0 - 0.0) + 0.0;

	vec4 X = vec4( 1.0, new_x, pow(new_x, 2.0), pow(new_x,3.0) );
	vec4 Y = vec4( 1.0, new_y, pow(new_y, 2.0), pow(new_y,3.0) );
		
	vec4 temp = X * coeffsX;
	dx = dot(temp, Y) * near_far_bias.z;
	temp = X * coeffsY;
	dy = dot(temp, Y) * near_far_bias.z;

	//FIXME: proc tady jsou 2/3 ???
	dx = dx;
	dy = dy;
	
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
	//f_values = _GetFuncValuesFromCell( grid_coords, D_X );
	
	//FIXME: Nefunguje!?!? vec4 t = textureGather( funcTex, vec2(0,0));

	f_values.x = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,0) ).r;
	f_values.y = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,0) ).r;
	f_values.z = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,1) ).r;
	f_values.w = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,1) ).r;


	M = mat2( f_values.xy, f_values.zw);

	temp = X * M;
	dx = dot(temp, Y) * near_far_bias.z;

	//-- diff Y
	f_values.x = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,0) ).g;
	f_values.y = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,0) ).g;
	f_values.z = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(0,1) ).g;
	f_values.w = textureOffset( funcTex, grid_coords/grid_res + 0.5/grid_res, ivec2(1,1) ).g;

	M = mat2(f_values.xy, f_values.zw);

	temp = X * M;
	dy = dot(temp, Y) * near_far_bias.z;

	//-- dx a dy se vztahuji k intervalu [0..1]. My to vsak pricitam k souradnicim, ktery je v intervalu [-1..1], tedy 2x vetsim.
	dx *= 2.0;
	dy *= 2.0;

	//------------------------------------------------------------------------------------

#endif
	vertexEyeSpace.x += dx;
	vertexEyeSpace.y += dy;

	res = f_values;
    gl_Position = vertexEyeSpace;
}
