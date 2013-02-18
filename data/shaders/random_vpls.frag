uniform samplerCube aliaserr_texture_cube_color;
uniform samplerCube aliaserr_texture_cube_pos;
uniform samplerCube aliaserr_texture_cube_norm;

uniform float seed;

in vec2 fragTexCoord;
out vec4 out_fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 coord; 
	float c = (fragTexCoord.x * 2048.0) * seed;

	coord.x = rand(vec2(c));
	coord.y = rand(vec2(fract(c/128.0)));
	coord.z = rand(vec2(floor(c/128.0)));

	coord = normalize( coord*2.0 - 1.0 );
	
	vec3 result;
	if( (fragTexCoord.y*3.0) < 1.0 )
		result = texture( aliaserr_texture_cube_color, coord ).rgb;
	else if ( (fragTexCoord.y*3.0) < 2.0 )
		result = texture( aliaserr_texture_cube_pos, coord ).rgb;
	else
		result = texture( aliaserr_texture_cube_norm, coord ).rgb;

	out_fragColor.xyz = result;
	out_fragColor.a = 1.0;
}