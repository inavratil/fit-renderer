
uniform mat4 lightViewMatrix;
uniform vec2 near_far;

in vec2 fragTexCoord;
in vec4 cam_position, light_position;
in vec4 o_vertex;

out vec4 out_FragData;

#define POLY_OFFSET 100.0

vec3 DPCoords()
{
    vec4 texCoords;
    texCoords = lightViewMatrix * o_vertex;
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

void main()
{
    vec4 c_pos = cam_position/cam_position.w;
	c_pos = c_pos * 0.5 + 0.5;
	c_pos.xy = round( c_pos.xy * 1024.0 );
	vec4 l_pos = vec4( DPCoords(), 1.0);
	//vec4 l_pos = light_position * 0.5 + 0.5;
	l_pos.xy = round( l_pos.xy * 1024.0 );

    out_FragData = vec4( c_pos.xy, l_pos.z, 1.0 );
}
