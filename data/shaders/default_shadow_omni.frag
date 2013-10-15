//Fragment shader for dual-paraboloid mapping
in vec4 position;
in vec2 fragTexCoord;


void main(void)
{
        gl_FragDepth = position.z;
}
