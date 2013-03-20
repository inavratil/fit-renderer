// Author: Juraj Vanek
// Color output for screen aligned-quad

in vec2 fragTexCoord;
layout ( location = 0 ) out vec4 out_FragColor;
layout ( location = 1 ) out vec4 out_FragMask;
uniform sampler2D tex_coords;
uniform sampler2D tex_error;
//uniform sampler2D tex_coverage;

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;
const float SM_X = 128.0;
const float SM_Y = 128.0;

const float MAX_MIPLEVEL = 6; // TODO: jako uniform ??

bool IsInsideFrustum( vec2 _screenPos )
{
	return (lessThan(_screenPos.xy, vec2(1.0) ) == bvec2(1.0) && greaterThan(_screenPos.xy, vec2(0.0) )== bvec2(1.0));
}

void main()
{
    vec4 a, b, c, d;

    a = textureOffset(tex_coords, fragTexCoord, ivec2(0,0) );
	if(a.r > 90.0) discard;
    b = textureOffset(tex_coords, fragTexCoord, ivec2(1,0) );
    c = textureOffset(tex_coords, fragTexCoord, ivec2(1,1) );
    d = textureOffset(tex_coords, fragTexCoord, ivec2(0,1) );

	//a = texture(tex_coords, fragTexCoord + vec2( 0.5/128.0 ) );
	//if(a.r > 90.0) discard;				
    //b = texture(tex_coords, fragTexCoord + vec2(1.0/128.0, 0.0) + vec2( 0.5/128.0 ) );
    //c = texture(tex_coords, fragTexCoord + vec2(1.0/128.0) + vec2( 0.5/128.0 ) );
    //d = texture(tex_coords, fragTexCoord + vec2(0.0, 1.0/128.0) + vec2( 0.5/128.0 ) );
	
	if( b.r > 90.0 || c.r > 90.0 || d.r > 90.0 )
		out_FragColor = vec4( 0.0, 1.0, 1.0, 0.0 );
	else
	{
    //-----

    vec3 ac = vec3( c.xy-a.xy, 0.0 );
    vec3 bd = vec3( d.xy-b.xy, 0.0 );
    ac.x *= SCREEN_X;
    ac.y *= SCREEN_Y;
    bd.x *= SCREEN_X;
    bd.y *= SCREEN_Y;
	//vypocet obsah ctyruhelniku - quadrilateral
    float K = 0.5 * abs( cross(ac, bd).z ); // zde by melo byt length misto abs, ale jelikoz z-ove 0, tak vysledek je pouze hodnota v z-ove ose
    //float K = max( length(ac+bd), length(ac-bd) );

    vec3 dp_ac = vec3( c.zw-a.zw, 0.0 );
    vec3 dp_bd = vec3( d.zw-b.zw, 0.0 );
    dp_ac.x *= SM_X;
    dp_ac.y *= SM_Y;
    dp_bd.x *= SM_X;
    dp_bd.y *= SM_Y;
    float dp_K = 0.5 * abs( cross(dp_ac, dp_bd).z ); // zde by melo byt length misto abs, ale jelikoz z-ove 0, tak vysledek je pouze hodnota v z-ove ose
    //float dp_K = max( length(dp_ac+dp_bd), length(dp_ac-dp_bd) );

	vec2 ds = vec2((b.x-a.x),(d.x-a.x)) * 128.0;
	vec2 dt = vec2((b.y-a.y),(d.y-a.y)) * 128.0;
	//ds = vec2(dFdx(a.x), dFdy(a.x)) * 128.0;
    //dt = vec2(dFdx(a.y), dFdy(a.y)) * 128.0;
//------------------------------------------------------------------------------
//-- TEST: kompenzace chybi podle pokryti solid angle jednim pixelem v shadow mape

    //vec2 x = a.zw;
    //x = x*2.0 - 1.0;
    //float l = length(x);
    //float index = 100.0 - floor(l*100.0); 
    //float cover = texture( tex_coverage, vec2(index,0.0) ).x;
    //dp_K = dp_K*cover;

//------------------------------------------------------------------------------
    //float error = K/dp_K;
	//float error = determinant( mat2(ds, dt) );
	float error = max( length(ds+dt), length(ds-dt) );

    float res_error = clamp(error, 1.0/11.0, 11.0);

    if( res_error < 1.0 )

        res_error = (res_error - 1.0/11.0) / (1.0 - 1.0/11.0) * 0.5; //-- from [1/11..11] to [0..1]
    else
        res_error = (res_error - 1.0) / (11.0 - 1.0) * (1.0 - 0.5) + 0.5; //-- from [1/11..11] to [0..1]

    vec3 color = texture( tex_error, vec2(res_error,0.0) ).xyz;

//------------------------------------------------------------------------------
//-- vypocet mozneho pokryti pixelu, aby chyba byla ~1.0


    //float new_res = min( MAX_MIPLEVEL, round( log2( sqrt( error ) ) ) ); //sqrt - jaka ma byt nova hrana ctverce, log2 - pro to nejblizsi mocnina 2 (kvuli mipmape), round - pro zaokrouhleni na nejblizsi
	float new_res = error; // sqrt( error );

//------------------------------------------------------------------------------

	
	out_FragColor = vec4( 0.0 );
	out_FragMask = vec4( 0.0 );

	float depth_thres = 15.0;
	
	if( IsInsideFrustum( a.xy ) )
	//if( length(ac)>depth_thres || length(bd)>depth_thres )
	{
		out_FragMask = vec4( 1.0 );
	}
		
		//if( new_res > 10.0)
		//	out_FragColor = vec4( 0.0, 1.0, 1.0, new_res );
		//else
			out_FragColor = vec4( color, new_res );
	}

	//out_FragColor = vec4(fragTexCoord.xy*128.0, 0.0, 1.0);
}
