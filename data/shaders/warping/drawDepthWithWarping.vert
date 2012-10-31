//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;

uniform mat4 in_ModelViewMatrix;

uniform vec2 near_far; // near and far plane for cm-cams

out vec2 fragTexCoord;
out vec4 position;
out vec4 res;

#define POLY_OFFSET 100.0

uniform mat4 coeffsX;
uniform mat4 coeffsY;

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;

const int INSIDE = 0;	//0x0000;
const int TOP = 1;		//0x0001;
const int BOTTOM = 2;	//0x0010;
const int RIGHT = 4;	//0x0100;
const int LEFT = 8;		//0x1000;

int computeCode(vec2 _p, vec4 range)
{
        int ret;
 
        ret = INSIDE;       
 
        if (_p.x < range.x)           // to the left of clip window
                ret |= LEFT;
        else if (_p.x > range.y)      // to the right of clip window
                ret |= RIGHT;
        if (_p.y < range.z)           // below the clip window
                ret |= BOTTOM;
        else if (_p.y > range.w)      // above the clip window
                ret |= TOP;
 
        return ret;
}

void main(void)
{
    fragTexCoord = in_Coord.xy;

    vec4 vertexEyeSpace = in_ModelViewMatrix * vec4(in_Vertex,1.0);
    gl_ClipDistance[0] = -vertexEyeSpace.z;
    
    float Length = length( vertexEyeSpace.xyz );
    vertexEyeSpace.z *= -1.0;
    vertexEyeSpace.xyz = normalize( vertexEyeSpace.xyz );

    vertexEyeSpace.z += 1.0;
    vertexEyeSpace.xy /= vertexEyeSpace.z;

    vertexEyeSpace.z = (Length - near_far.x)/(near_far.y  - near_far.x);
    vertexEyeSpace.w = 1.0; 

	position = vertexEyeSpace;


	//------------------------------------------------------------------------------------
	// Polynomial warping

	res = vec4( 0.0 );

	vec4 range = vec4( 41.0, 83.0, 11.0, 41.0 );

	vec2 p = position.xy;
	p = p*0.5 + 0.5;
	p = p * vec2( 128.0, 128.0 );

	int code = computeCode(p, range);

	float dx,dy;
	if ( code == INSIDE )
	{
		float new_x = (p.x - range.x)/(range.y - range.x) * (3.0 - 0.0) + 0.0;
		float new_y = (p.y - range.z)/(range.w - range.z) * (3.0 - 0.0) + 0.0;

		vec4 X = vec4( 1.0, new_x, pow(new_x, 2.0), pow(new_x,3.0) );
		vec4 Y = vec4( 1.0, new_y, pow(new_y, 2.0), pow(new_y,3.0) );
		
		vec4 temp = X * coeffsX;
		dx = dot(temp, Y) * 25.0/1024.0;
		temp = X * coeffsY;
		dy = dot(temp, Y) * 25.0/1024.0;
		//dx = dx * 2.0 - 1.0;
		//dy = dy * 2.0 - 1.0;

		vertexEyeSpace.x += dx;
		vertexEyeSpace.y += dy;
	}

#if 0
	vec4 P = vec4( 0.0 );
	float new_val = 0.0, new_range = 0.0;

	if( code == TOP )
	{
		P = textureOffset( polynomials, vec2(0.0), ivec2(3,0) );
		new_range = P.x*pow(p.x,3.0) + P.y*pow(p.x,2.0) + P.z*p.x + P.w;
	
		new_val = (p.y - 41.0)/(128.0 - 41.0) * (128.0 - new_range) + new_range;
		new_val = new_val/128.0;
		new_val = new_val*2.0 - 1.0;
		vertexEyeSpace.y = new_val;
		res.a = 1.0;
	}

	else if ( code == BOTTOM )
	{
		P = textureOffset( polynomials, vec2(0.0), ivec2(0,0) );
		new_range = P.x*pow(p.x,3.0) + P.y*pow(p.x,2.0) + P.z*p.x + P.w;
	
		new_val = (p.y - 0.0)/(11.0 - 0.0) * (new_range - 0.0) + 0.0;
		new_val = new_val/128.0;
		new_val = new_val*2.0 - 1.0;
		vertexEyeSpace.y = new_val;
	}
	else if ( code == RIGHT )
	{
		P = textureOffset( polynomials, vec2(0.0), ivec2(7,0) );
		new_range = P.x*pow(p.y,3.0) + P.y*pow(p.y,2.0) + P.z*p.y + P.w;

		new_val = (p.x - 83.0)/(128.0 - 83.0) * (128.0 - new_range) + new_range;
		new_val = new_val/128.0;
		new_val = new_val*2.0 - 1.0;
		vertexEyeSpace.x = new_val;
	}
	else if ( code == LEFT )
	{
		P = textureOffset( polynomials, vec2(0.0), ivec2(4,0) );
		new_range = P.x*pow(p.y,3.0) + P.y*pow(p.y,2.0) + P.z*p.y + P.w;
	
		new_val = (p.x - 0.0)/(41.0 - 0.0) * (new_range - 0.0) + 0.0;
		new_val = new_val/128.0;
		new_val = new_val*2.0 - 1.0;
		vertexEyeSpace.x = new_val;
	}
	else if ( code == INSIDE )
	{
		// urci indexy bunky v mrizce, ve ktere se vertex nachazi
		vec2 grid_index = floor( vec2( (p.x-41.0)/14.0, (p.y-11.0)/10.0 ) ); // (px-min) / width/3

		// vyber pro danou bunku indexy polynomu, ktere se nachazi ve vertikalni a v horizontalnim smeru
		vec2 v = vec2( 0.0, 1.0 ) + vec2(grid_index.y); //vertical - radek 
		vec2 h = vec2( 4.0, 5.0 ) + vec2(grid_index.x); //horizontal sloupec

		// z textur nacti koeficinety polynomu pro oba smery
		vec4 P_v_bottom = textureOffset( polynomials, vec2(0.0), ivec2(v.x, 0.0) );
		vec4 P_v_top = textureOffset( polynomials, vec2(0.0), ivec2(v.y, 0.0) );
		vec4 P_h_left = textureOffset( polynomials, vec2(0.0), ivec2(h.x, 0.0) );
		vec4 P_h_right = textureOffset( polynomials, vec2(0.0), ivec2(h.y, 0.0) );

		// podle polynomu se vypocitaji nove rozsahy pro danou bunku
		float new_bottom = P_v_bottom.x*pow(p.x,3.0) + P_v_bottom.y*pow(p.x,2.0) + P_v_bottom.z*p.x + P_v_bottom.w;
		float new_top = P_v_top.x*pow(p.x,3.0) + P_v_top.y*pow(p.x,2.0) + P_v_top.z*p.x + P_v_top.w;
		float new_left = P_h_left.x*pow(p.y,3.0) + P_h_left.y*pow(p.y,2.0) + P_h_left.z*p.y + P_h_left.w;
		float new_right = P_h_right.x*pow(p.y,3.0) + P_h_right.y*pow(p.y,2.0) + P_h_right.z*p.y + P_h_right.w;

		// puvodni rozsahy pro danou bunku
		vec2 range_h = vec2( 41.0, 55.0 ) + vec2(grid_index.x*14.0); //j-ty sloupec
		vec2 range_v = vec2( 11.0, 21.0 ) + vec2(grid_index.y*10.0); //i-ty radek
		

		float new_x = (p.x - range_h.x)/(range_h.y - range_h.x) * (new_right - new_left) + new_left;
		float new_y = (p.y - range_v.x)/(range_v.y - range_v.x) * (new_top - new_bottom) + new_bottom;

		vec2 new_coords = (vec2(new_x, new_y)/128.0)*2.0 - 1.0;

		vertexEyeSpace.xy = new_coords;
	}

	//------------------------------------------------------------------------------------
#endif
	
	res = vertexEyeSpace;
    gl_Position = vertexEyeSpace;
}
