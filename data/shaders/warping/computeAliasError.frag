// Author: Juraj Vanek
// Color output for screen aligned-quad

in vec2 fragTexCoord;
out vec4 out_FragColor;
uniform sampler2D tex_coords;
uniform sampler2D tex_error;
//uniform sampler2D tex_coverage;

const float SCREEN_X = 1024.0;
const float SCREEN_Y = 1024.0;
const float SM_X = 1024.0;
const float SM_Y = 1024.0;

const float MAX_MIPLEVEL = 6; // TODO: jako uniform ??

void main()
{
    vec4 a, b, c, d;

    a = textureOffset(tex_coords, fragTexCoord, ivec2(0,0) );
	if(a.r > 90.0) discard;
    b = textureOffset(tex_coords, fragTexCoord, ivec2(1,0) );
    c = textureOffset(tex_coords, fragTexCoord, ivec2(1,1) );
    d = textureOffset(tex_coords, fragTexCoord, ivec2(0,1) );

    //-----

    vec3 ac = vec3( c.xy-a.xy, 0.0 );
    vec3 bd = vec3( d.xy-b.xy, 0.0 );
    ac.x *= SCREEN_X;
    ac.y *= SCREEN_Y;
    bd.x *= SCREEN_X;
    bd.y *= SCREEN_Y;
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

//------------------------------------------------------------------------------
//-- TEST: kompenzace chybi podle pokryti solid angle jednim pixelem v shadow mape

    //vec2 x = a.zw;
    //x = x*2.0 - 1.0;
    //float l = length(x);
    //float index = 100.0 - floor(l*100.0); 
    //float cover = texture( tex_coverage, vec2(index,0.0) ).x;
    //dp_K = dp_K*cover;

//------------------------------------------------------------------------------

    float res_error = clamp(K/dp_K, 1.0/11.0, 11.0);

    if( res_error < 1.0 )

        res_error = (res_error - 1.0/11.0) / (1.0 - 1.0/11.0) * 0.5; //-- from [1/11..11] to [0..1]
    else
        res_error = (res_error - 1.0) / (11.0 - 1.0) * (1.0 - 0.5) + 0.5; //-- from [1/11..11] to [0..1]

    vec3 color = texture( tex_error, vec2(res_error,0.0) ).xyz;

//------------------------------------------------------------------------------
//-- vypocet mozneho pokryti pixelu, aby chyba byla ~1.0

    float error = K/dp_K;
    //float new_res = min( MAX_MIPLEVEL, round( log2( sqrt( error ) ) ) ); //sqrt - jaka ma byt nova hrana ctverce, log2 - pro to nejblizsi mocnina 2 (kvuli mipmape), round - pro zaokrouhleni na nejblizsi
	float new_res = sqrt( error );

//------------------------------------------------------------------------------

	//if( (a.x >= 0.0 && a.x <= 1.0) && (a.y >= 0.0 && a.y <= 1.0) )
	//if( dp_K == 0.0 || K == 0.0)
	//	out_FragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
	//else
		out_FragColor = vec4( color, new_res );
}