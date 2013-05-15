// Aliaserr.frag - multires
in vec2 fragTexCoord;
in vec4 o_vertex;
in vec4 light_vertex;

layout( location = 0 ) out vec4 out_fragColor;
layout( location = 1 ) out vec4 out_fragMask;

uniform mat4 cam_mv, cam_proj;
uniform mat4 in_ModelViewMatrix;
uniform mat4 matrix_ortho;
uniform vec3 near_far_bias; // near and far plane for cm-cams
uniform vec4 camera_space_light_position;

#define POLY_OFFSET 100.0

bool IsInsideFrustum( vec2 _screenPos )
{
	return (lessThan(_screenPos.xy, vec2(1.0) ) == bvec2(1.0) && greaterThan(_screenPos.xy, vec2(0.0) )== bvec2(1.0));
}

//-----------------------------------------------------------------------------

vec4 GetRotationQuat( vec3 _from, vec3 _to )
{
    vec4 q;

    vec3 v0 = normalize( _from );
    vec3 v1 = normalize( _to );

    float d = dot( v0, v1 );
    float s = 1.0 / sqrt( (1.0+d)*2.0 );
    vec3 c = cross( v0, v1 );

    q.x = c.x * s;
    q.y = c.y * s;
    q.z = c.z * s;
    q.w = s * 0.5f;

    return normalize( q );
}

mat4 QuatToMatrix( vec4 q )
{
    mat4 Result = mat4( 1.0 );
    Result[0][0] = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
    Result[0][1] = 2 * q.x * q.y + 2 * q.w * q.z;
    Result[0][2] = 2 * q.x * q.z - 2 * q.w * q.y;

    Result[1][0] = 2 * q.x * q.y - 2 * q.w * q.z;
    Result[1][1] = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
    Result[1][2] = 2 * q.y * q.z + 2 * q.w * q.x;

    Result[2][0] = 2 * q.x * q.z + 2 * q.w * q.y;
    Result[2][1] = 2 * q.y * q.z - 2 * q.w * q.x;
    Result[2][2] = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
    return Result;
}

vec3 DPCoords( vec4 _position )
{
    vec4 texCoords;

    texCoords = in_ModelViewMatrix * _position;

    //texCoords.xyz /= texCoords.w;
    float Length = length( texCoords.xyz );

    texCoords.z *= -1.0;
    texCoords.xyz = normalize( texCoords.xyz );

    texCoords.z += 1.0;
    texCoords.x /= texCoords.z;
    texCoords.y /= texCoords.z;

    texCoords.z = (Length - near_far_bias.x)/(near_far_bias.y + POLY_OFFSET - near_far_bias.x);
    texCoords.w = 1.0;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}

vec3 OrthoCoords( vec4 _position )
{
	vec4 coords;
	coords = matrix_ortho * in_ModelViewMatrix * _position;

	return vec3( 0.5*coords.xy + 0.5, coords.z);
}

//-----------------------------------------------------------------------------

void main() 
{
    vec4 cam_coords, cam_eye, dp_coords;
	float w;

	cam_eye = cam_mv * o_vertex;
    cam_coords = cam_proj * cam_eye;
	w = cam_coords.w;
    cam_coords = cam_coords/cam_coords.w;
    cam_coords.xy = cam_coords.xy * 0.5 + 0.5;

	if(w < 0.0 || !IsInsideFrustum(cam_coords.xy) )
		discard;


//-----------------------------------------------------------------------------
	const float TWO_TAN_TH = 0.828427; // FOV = 45'
	const vec3 AXIS_Z = vec3( 0, 0, 1 );
	float wi = (TWO_TAN_TH/128.0)*( length(cam_eye.xyz) ); //-- v a.x je ulozena vzdalenost od kamery

	//Dual-Paraboloid:
	vec3 light_direction	= normalize( camera_space_light_position.xyz - cam_eye.xyz ); //-- compute directional vector to the light
	//Ortho:
	//vec3 light_direction	= normalize( vec3(0,1,1) ); //-- compute directional vector to the light
	vec3 camera_direction	= normalize( -cam_eye.xyz ); //-- compute directional vector to the camera
	vec3 point_normal		= normalize( camera_direction + light_direction );

	mat4 quad_rotation_matrix = QuatToMatrix( GetRotationQuat( AXIS_Z, point_normal ) );
	vec4 rotated_points[4] = vec4[] (
		quad_rotation_matrix * vec4( -wi/2, -wi/2, 0.0, 1.0 ),
		quad_rotation_matrix * vec4(  wi/2, -wi/2, 0.0, 1.0 ),
		quad_rotation_matrix * vec4(  wi/2,  wi/2, 0.0, 1.0 ),
		quad_rotation_matrix * vec4( -wi/2,  wi/2, 0.0, 1.0 )
	);
	//-- point light - Dual-Paraboloid mapping
	vec2 a = DPCoords( inverse(cam_mv) * vec4(cam_eye.xyz + rotated_points[0].xyz, 1.0) ).xy;
	vec2 b = DPCoords( inverse(cam_mv) * vec4(cam_eye.xyz + rotated_points[1].xyz, 1.0) ).xy;
	vec2 c = DPCoords( inverse(cam_mv) * vec4(cam_eye.xyz + rotated_points[2].xyz, 1.0) ).xy;
	vec2 d = DPCoords( inverse(cam_mv) * vec4(cam_eye.xyz + rotated_points[3].xyz, 1.0) ).xy;
	
	/*
	vec2 a = OrthoCoords( o_vertex + rotated_points[0] ).xy;
	vec2 b = OrthoCoords( o_vertex + rotated_points[1] ).xy;
	vec2 c = OrthoCoords( o_vertex + rotated_points[2] ).xy;
	vec2 d = OrthoCoords( o_vertex + rotated_points[3] ).xy;
	*/
	vec3 ac = vec3( c.xy-a.xy, 0.0 );
    vec3 bd = vec3( d.xy-b.xy, 0.0 );
    ac.x *= 128.0; //SM_RES;
    ac.y *= 128.0; //SM_RES;
    bd.x *= 128.0; //SM_RES;
    bd.y *= 128.0; //SM_RES;
	//vypocet obsah ctyruhelniku - quadrilateral
    float K = 0.5 * abs( cross(ac, bd).z ); // zde by melo byt length misto abs, ale jelikoz z-ove 0, tak vysledek je pouze hodnota v z-ove ose
    //float K = max( length(ac+bd), length(ac-bd) );

//-----------------------------------------------------------------------------
	//if(w<0.0)
	//	cam_coords.xy = vec2(99.0, 0.0);

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

	out_fragColor = vec4( 0.25/K, cam_eye.x, dp_coords.xy );
    //out_fragColor = vec4( cam_coords.xy, dp_coords.xy );
	out_fragMask = vec4( 1.0 );
}
