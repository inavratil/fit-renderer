in vec2 fragTexCoord;
in vec4 position;
in vec4 res;

out vec4 out_FragData;



void main()
{
	out_FragData = vec4(position.z*10.0);
}
