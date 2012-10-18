layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;

out vec3 vPosition;
//out vec3 vNormal;

void main()
{
      //vNormal = in_Normal;
	  vPosition = in_Vertex;
}
