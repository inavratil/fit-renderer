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

//-- Polynomial uniforms
uniform mat4 coeffsX;
uniform mat4 coeffsY;

//-- Bilinear uniforms
uniform sampler2D funcTex;


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
	p = p * vec2( SCREEN_X, SCREEN_Y );	//-- p nabyva hodnot [0..128]

	//------------------------------------------------------------------------------------
	//-- Polynomial warping


	float new_x = (p.x - range.x)/(range.y - range.x) * (3.0 - 0.0) + 0.0;
	float new_y = (p.y - range.z)/(range.w - range.z) * (3.0 - 0.0) + 0.0;

	vec4 X = vec4( 1.0, new_x, pow(new_x, 2.0), pow(new_x,3.0) );
	vec4 Y = vec4( 1.0, new_y, pow(new_y, 2.0), pow(new_y,3.0) );
		
	vec4 temp = X * coeffsX;
	dx = dot(temp, Y) * near_far_bias.z;
	temp = X * coeffsY;
	dy = dot(temp, Y) * near_far_bias.z;

	//FIXME: proc tady jsou 2/3 ???
	dx = dx*2.0/3.0;
	dy = dy*2.0/3.0;

	vertexEyeSpace.x += dx;
	vertexEyeSpace.y += dy;
	
	//------------------------------------------------------------------------------------

	res = vertexEyeSpace;
    gl_Position = vertexEyeSpace;
}
