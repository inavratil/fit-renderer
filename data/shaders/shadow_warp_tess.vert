//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;

out vec4 vPosition;
out vec2 vCoord;

//#define PARABOLA_CUT

void main(void)
{
    vCoord = in_Coord.xy;
    vPosition = vec4(in_Vertex, 1.0);
}
