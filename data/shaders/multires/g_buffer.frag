uniform int alpha_test;
uniform sampler2D alpha_tex;

in vec2 fragTexCoord;
in vec3 eyeNorm;
in vec4 eyePos;

out vec4 out_FragData[3];

void OutputGBuffer()
{
    //lambert term
	float NdotV = dot(normalize(eyeNorm), -normalize(eyePos.xyz));
	//color (TODO: do we really need it?)
	out_FragData[0].xyz = vec3(NdotV);
    if(NdotV < 0) 
		out_FragData[0].w = -NdotV;
    else
		out_FragData[0].w = NdotV;


    //surface normal
	if(NdotV < 0)
		out_FragData[1].xyz = -normalize(eyeNorm);
    else
		out_FragData[1].xyz = normalize(eyeNorm);
    //linear space depth
	out_FragData[1].w = length(eyePos);


    //eye-space position + linear space depth again
	out_FragData[2].xyz = eyePos.xyz;
    out_FragData[2].w = length(eyePos);
}


void main()
{
    //alpha test
    if(alpha_test == 1)
    {
        vec3 alpha_value = texture(alpha_tex, fragTexCoord).rgb;
        if( all(lessThan(alpha_value, vec3(0.25))) )
            discard;
        else        	   
            OutputGBuffer();
    }
    else
		OutputGBuffer();
}
