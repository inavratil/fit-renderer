// Aliaserr.frag - multires
in vec2 fragTexCoord;
in vec4 o_vertex;
in vec4 light_vertex;

out vec4 out_fragColor;

uniform mat4 cam_mv, cam_proj;
uniform mat4 lightMatrix;

void main() 
{
    vec4 cam_coords, cam_eye, dp_coords;

	cam_eye = cam_mv * o_vertex;
    cam_coords = cam_proj * cam_eye;
    cam_coords = cam_coords/cam_coords.w;
    cam_coords.xy = cam_coords.xy * 0.5 + 0.5;
	if( cam_eye.z > 0.0 )
		cam_coords.xy = vec2( 99.0, 0.0 );

	/*
	 * per-pixel presny vypocet chyby neni potreba, protoze ve vysledku se to stejne nedela.
	 * Transformuji se jen vrcholy a vnitrek trojuhelniku se interpoluje
    dp_coords = lightMatrix * o_vertex;
    dp_coords.z *= -1.0;
    dp_coords.xyz = normalize( dp_coords.xyz );
    dp_coords.z += 1.0;
    dp_coords.xy /= dp_coords.z;
    dp_coords.xy = 0.5*dp_coords.xy + 0.5;
	*/

	dp_coords.xy = 0.5*light_vertex.xy + 0.5;

    out_fragColor = vec4( cam_coords.xy, dp_coords.xy );
}
