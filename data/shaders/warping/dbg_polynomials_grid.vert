// Author: Juraj Vanek
// Only basic vertex transformation for screen aligned-quad

//vertex attribute
layout(location = 0) in vec2 in_ScreenCoord;
layout(location = 1) in vec3 in_Color;
out vec3 fragColor;

void main()
{
	gl_Position = vec4(in_ScreenCoord, 0.0, 1.0);
	fragColor = in_Color;
}
