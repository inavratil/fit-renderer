out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D grid_points;
uniform sampler2D gradient_map;

uniform vec2 scaleRes;


void main()
{
	vec4 dxdy;
	vec4 res, coords;

	coords = texture( grid_points, fragTexCoord );

    dxdy.rg = texture(gradient_map, coords.xy/scaleRes.y ).rg;
	dxdy.ba = texture(gradient_map, coords.zw/scaleRes.y ).rg;

	res = coords + scaleRes.x*dxdy;
	out_FragColor = res;
}
