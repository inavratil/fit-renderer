uniform sampler2DArray show_depth_dpShadowA;

in vec2 fragTexCoord;
out vec4 out_fragData;

uniform float index;
uniform float far_plane;

void main()
{
    float depth = texture(show_depth_dpShadowA, vec3(fragTexCoord, index)).r;
    out_fragData = vec4(10.0*depth );
}

