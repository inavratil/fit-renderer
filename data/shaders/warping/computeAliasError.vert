// Author: Juraj Vanek
// Only basic vertex transformation for screen aligned-quad

//vertex attribute
layout(location = 0) in vec2 in_ScreenCoord;
out vec2 fragTexCoord;

void main()
{
	gl_Position = vec4(in_ScreenCoord, 0.0, 1.0);
	fragTexCoord = 0.5 * in_ScreenCoord + 0.5;
}
