//Fragment shader for dual-paraboloid mapping
in vec4 tePosition;
in vec2 teCoord;

uniform int alpha_test;
uniform sampler2D alpha_tex;

//out vec4 out_FragColor;

void main(void)
{
    //alpha test
    if(alpha_test == 1)
    {
        vec3 alpha_value = texture(alpha_tex, teCoord).rgb;
        if( all(lessThan(alpha_value, vec3(0.25))) )
            discard;
        else
            // write depth
            gl_FragDepth = tePosition.z;
            //out_FragColor = vec4(1.0);
    }
    else
        gl_FragDepth = tePosition.z;
        //out_FragColor = vec4(1.0);
}
