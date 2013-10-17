//Fragment shader for dual-paraboloid mapping
in vec4 position;
in vec2 fragTexCoord;

uniform int alpha_test;
uniform sampler2D alpha_tex;

void main(void)
{
    //alpha test
    if(alpha_test == 1)
    {
        vec3 alpha_value = texture(alpha_tex, fragTexCoord).rgb;
        if( all(lessThan(alpha_value, vec3(0.25))) )
            discard;
        else
            // write depth
            gl_FragDepth = position.z;
    }
    else
        gl_FragDepth = position.z;
}