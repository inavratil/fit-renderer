in vec3 fragColor;
out vec4 out_FragColor;

void main()
{
	out_FragColor = vec4( fragColor, 1.0 );
}
