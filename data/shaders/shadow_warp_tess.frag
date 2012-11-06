//Fragment shader for dual-paraboloid mapping
in vec4 tePosition;
in vec2 teCoord;


void main(void)
{
        gl_FragDepth = tePosition.z;
}
