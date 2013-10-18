uniform sampler2D mat_aliasErrorBaseA;

uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid
uniform vec2 near_far; // near and far plane for cm-cams

in  vec4 o_vertex;
out vec4 out_fragColor;

#define POLY_OFFSET 100.0
#define SM_RES 1024.0

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

    texCoords.z = (Length - near_far.x)/(near_far.y + POLY_OFFSET - near_far.x);
    texCoords.w = 1.0;

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

    texCoords.z = (Length - near_far.x)/(near_far.y + POLY_OFFSET - near_far.x);
    texCoords.w = 1.0;

    return vec3( 0.5*texCoords.xy + 0.5, texCoords.z);
}
void main(void)
{
    vec3 front_coords = DPCoordsFront();
    vec3 back_coords = DPCoordsBack();

    vec4 vertexLightSpace = lightModelView[0] * o_vertex;
    float split_plane = -vertexLightSpace.z;

    vec4 color_result = vec4( 0.0 );
    vec3 curr_texCoords;
    if(split_plane >= 0.0)
        curr_texCoords = front_coords;
    else
        curr_texCoords = back_coords;

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

    vec3 c = texture( mat_aliasErrorBaseA, vec2(res_error,0.0) ).xyz;
    color_result = vec4( c, 1.0 );

    //-- grid   
#if 1
    float gridLineWidth = 1.0;

    vec2 scaledTC = curr_texCoords.xy / (20.0 * 1.0/1024.0);
    vec2 dS = vec2(dFdx(scaledTC.s),dFdy(scaledTC.s));
    vec2 dT = vec2(dFdx(scaledTC.t),dFdy(scaledTC.t));
    vec2 m = fract( scaledTC );
    if( m.s < gridLineWidth*length(dS) || m.t < gridLineWidth*length(dT) )
        color_result = vec4( 1.0, 0.0, 0.0, 1.0 );
#endif
    //-- End

    out_fragColor = color_result;
    out_fragColor.a = ma_error;

/*
    vec2 C[] = vec2[](
            vec2( -1.2568359, 3.7421875 ), 
            vec2( -1.2099609, 4.0429688 ), 
            vec2( -1.9521484, 3.4335938 ), 
            vec2( -1.7597656, 3.921875 ) 
            );
    if( length(abs(o_vertex.xy - vec2(C[0].x, C[0].y))) < length(vec2( 0.02 )) ) 
        out_fragColor = vec4( 0.0, 0.0, 1.0, 1.0 );
    if( length(abs(o_vertex.xy - vec2(C[1].x, C[1].y))) < length(vec2( 0.02 )) )
        out_fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    if( length(abs(o_vertex.xy - vec2(C[2].x, C[2].y))) < length(vec2( 0.02 )) ) 
        out_fragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
    if( length(abs(o_vertex.xy - vec2(C[3].x, C[3].y))) < length(vec2( 0.02 )) )
        out_fragColor = vec4( 1.0, 1.0, 0.0, 1.0 );
*/

}
