uniform sampler2D tex_error_color;
uniform sampler2D tex_2Dfunc_values;

uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid
uniform vec3 near_far_bias; // near and far plane for cm-cams
uniform vec4 range;
uniform mat4 in_ModelViewMatrix;

uniform vec4 camera_space_light_position;	//-- position of light in camera space

in  vec3 camera_space_position;
in  vec4 o_vertex;
out vec4 out_fragColor;

#define POLY_OFFSET 100.0

const float SM_RES = 128.0;
const float TWO_TAN_TH = 0.828427; // FOV = 45'

#define POLY_OFFSET 100.0

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;


////////////////////////////////////////////////////////////////////////////////

vec3 DPCoords( vec4 _position )
{
    vec4 texCoords;

    texCoords = lightModelView[0] * _position;

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


//-----------------------------------------------------------------------------

void main(void)
{
    

	vec4 vertexEyeSpace = in_ModelViewMatrix * o_vertex;	//-- in vec3 camera_space_position;
    vec4 vertexLightSpace = lightModelView[0] * o_vertex;
    float split_plane = -vertexLightSpace.z;

    vec4 color_result = vec4( 0.0 );
//-----------------------------------------------------------------------------	

	const vec3 AXIS_Z = vec3( 0, 0, 1 );
	float wi = (TWO_TAN_TH/SCREEN_X)*( -camera_space_position.z ); //-- minus, protoze osa Z smeruje za kameru

	vec3 light_direction	= normalize( camera_space_light_position.xyz - camera_space_position.xyz ); //-- compute directional vector to the light
	vec3 camera_direction	= normalize( -camera_space_position.xyz ); //-- compute directional vector to the camera
	vec3 point_normal		= normalize( camera_direction + light_direction );

	vec2 points[4] = vec2[] (
		vec2( -wi/2, -wi/2 ),
		vec2(  wi/2, -wi/2 ),
		vec2(  wi/2,  wi/2 ),
		vec2( -wi/2,  wi/2 )
	);

	mat4 quad_rotation_matrix = QuatToMatrix( GetRotationQuat( AXIS_Z, point_normal ) );
	vec4 rotated_points[4] = vec4[] (
		quad_rotation_matrix * vec4( points[0], 0.0, 1.0 ),
		quad_rotation_matrix * vec4( points[1], 0.0, 1.0 ),
		quad_rotation_matrix * vec4( points[2], 0.0, 1.0 ),
		quad_rotation_matrix * vec4( points[3], 0.0, 1.0 )
	);

	
	vec2 a = DPCoords( o_vertex + rotated_points[0] ).xy;
	vec2 b = DPCoords( o_vertex + rotated_points[1] ).xy;
	vec2 c = DPCoords( o_vertex + rotated_points[2] ).xy;
	vec2 d = DPCoords( o_vertex + rotated_points[3] ).xy;

	vec3 ac = vec3( c.xy-a.xy, 0.0 );
    vec3 bd = vec3( d.xy-b.xy, 0.0 );
    ac.x *= SM_RES;
    ac.y *= SM_RES;
    bd.x *= SM_RES;
    bd.y *= SM_RES;
	//vypocet obsah ctyruhelniku - quadrilateral
    float K = 0.5 * abs( cross(ac, bd).z ); // zde by melo byt length misto abs, ale jelikoz z-ove 0, tak vysledek je pouze hodnota v z-ove ose
    //float K = max( length(ac+bd), length(ac-bd) );

	color_result = vec4( K );	//FIXME: debug output
	color_result.a = 1.0;

//-----------------------------------------------------------------------------
#if 1

    float res_error = clamp(1/K, 1.0/11.0, 11.0);

    if( res_error < 1.0 )

        res_error = (res_error - 1.0/11.0) / (1.0 - 1.0/11.0) * 0.5; //-- from [1/11..11] to [0..1]
    else
        res_error = (res_error - 1.0) / (11.0 - 1.0) * (1.0 - 0.5) + 0.5; //-- from [1/11..11] to [0..1]

    color_result = vec4( texture( tex_error_color, vec2(res_error,0.0) ).xyz, 1.0 );

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