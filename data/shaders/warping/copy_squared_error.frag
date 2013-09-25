uniform sampler2D tex_alias_error;

in vec2 fragTexCoord;
out vec4 out_FragColor;

void main()
{
	vec4 value = texture( tex_alias_error, fragTexCoord );
	out_FragColor = vec4( value.rgb, value.a );
}
