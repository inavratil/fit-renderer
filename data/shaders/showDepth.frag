uniform sampler2D show_depthShadowA;

in vec2 fragTexCoord;
out vec4 out_fragData;
uniform float far_plane;

void main()
{
    float depth = texture(show_depthShadowA, fragTexCoord).r;
    out_fragData = vec4( 2.0/(far_plane - depth * far_plane) );
}

