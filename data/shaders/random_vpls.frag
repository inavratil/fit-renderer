uniform sampler2D random;


float seed;

in vec2 fragTexCoord;
out vec4 out_fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 rand = texture(random, fragTexCoord).rgb;
	out_fragColor = vec4( rand, 1.0 );
}