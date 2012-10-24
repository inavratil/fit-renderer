out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D tex;
uniform float scale;
uniform vec3 range; //from, to, size

void main()
{
	vec2 dxdy, coords;
	vec4 res;

	coords = fragTexCoord*(range.y - range.x)/range.z + range.x/range.z;

    dxdy = texture(tex, coords ).rg;

	res = scale * vec4(dxdy, 0.0, 1.0);
	out_FragColor = res;
}
