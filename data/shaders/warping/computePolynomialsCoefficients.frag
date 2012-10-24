out vec4 out_FragColor;
in vec2 fragTexCoord;

uniform sampler2D new_grid_points;

const ivec4 indices[8] = ivec4[](
	ivec4(0, 1, 2, 3),
	ivec4(4, 5, 6, 7),
	ivec4(8, 9, 10, 11),
	ivec4(12, 13, 14, 15),
	ivec4(0, 4, 8, 12),
	ivec4(1, 5, 9, 13),
	ivec4(2, 6, 10, 14),
	ivec4(3, 7, 11, 15)
);
void main()
{
	vec2 points[16];
	vec4 res, tmp;

	//FIXME: Tady to optimalizovat a cist jen ty, co jsou pro dany "pixel", aka polynom, potreba
	for(int i=0;i<8;i++)
	{
		tmp = textureOffset( new_grid_points, vec2( 0.0 ), ivec2(i,0) );
		points[i] = tmp.rg;
		points[i+8] = tmp.ba;
	}

	ivec4 index = indices[ int(floor(fragTexCoord.x * 8.0)) ];

	//-------------------------------------------------------------------------------------------
	mat4 M;
	vec2 a = points[index.x];
	vec2 b = points[index.y];
	vec2 c = points[index.z];
	vec2 d = points[index.w];

	vec4 X, Y;
#ifdef HORIZONTAL	
	X = vec4(a.x, b.x, c.x, d.x);
	Y = vec4(a.y, b.y, c.y, d.y);
#else
	Y = vec4(a.x, b.x, c.x, d.x);
	X = vec4(a.y, b.y, c.y, d.y);
#endif
	 
	M[3] = vec4(1.0);
	M[2] = X;
	M[1] = pow(X, vec4(2.0));
	M[0] = pow(X, vec4(3.0));
	
	res = Y*transpose(inverse(M));

	out_FragColor = res;
}
