// Author: Juraj Vanek
// Color output for progress bar

in vec2 fragTexCoord;
out vec4 out_FragColor;
uniform sampler2D tex;

void main()
{
	out_FragColor = texture(tex, fragTexCoord * vec2(1.0, 20.0));
}
