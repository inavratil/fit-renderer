// Author: Juraj Vanek
// Color output for screen aligned-quad

in vec2 fragTexCoord;
out vec4 out_FragColor;
uniform sampler2D tex;
uniform float lod;

void main()
{
	out_FragColor = textureLod(tex, fragTexCoord, lod);
}
