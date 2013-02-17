uniform sampler2D random;


float seed;

in vec2 fragTexCoord;
out vec4 out_fragColor;

void main()
{
	vec3 rand = texture(random, fragTexCoord).rgb;
	out_fragColor = vec4( rand, 1.0 );
}