out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D gradient_map;
uniform vec4 range; //-- udava rozmery a krokovani mrizky, ze ktere se budou koeficienty pocitat Y-ova osa .x,.y - zacatek/step, X-ova .z/.w - zacatek/step

void main()
{
	vec4 res, tmp;

	vec2 coords = floor(fragTexCoord*4.0);
	coords = vec2(range.z + coords.x*range.w, range.x + coords.y*range.y );

	tmp = texture( gradient_map, vec2( coords/128.0 ) + vec2( 0.5/128.0 ) );
	//tmp *= 15.0;

	out_FragColor = vec4(tmp);
}
