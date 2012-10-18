uniform int alpha_test;
uniform sampler2D alpha_tex;

in vec2 fragTexCoord;
out vec4 out_FragData;

void main()
{
    //alpha test
    if(alpha_test == 1)
    {
        vec3 alpha_value = texture(alpha_tex, fragTexCoord).rgb;
        if( all(lessThan(alpha_value, vec3(0.25))) )
            discard;
        else
            // write depth
            out_FragData = vec4(1.0);
    }
    else
        out_FragData = vec4(1.0);
}
