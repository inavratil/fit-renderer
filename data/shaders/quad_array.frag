in vec2 fragTexCoord;
out vec4 out_FragColor;

uniform sampler2DArray tex;
uniform float index;

void main()
{
	out_FragColor = texture(tex, vec3(fragTexCoord,index));
}
