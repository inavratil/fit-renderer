uniform sampler2D tex;
 
in vec2 fragTexCoord;
out vec4 out_fragData;

void main()
{
    vec4 c = texture(tex, fragTexCoord);
    //out_fragData = vec4(1.0, 0.0, 0.0, 1.0);
    out_fragData = c;
}

