//Select depth from RGBA render texture (depth is stored in alpha channel)
uniform sampler2D normal_texture;
in vec2 fragTexCoord;
out vec4 out_FragData;

void main()
{
    //output normal and depth values
    out_FragData = texture(normal_texture, fragTexCoord);
}
