//Bounding volumes fragment shader

layout(location = 0) in vec3 in_Vertex;

uniform mat4 in_ModelViewMatrix;

layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};

void main()
{
	vec4 vertex = vec4(in_Vertex,1.0);
	gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * vertex;
}