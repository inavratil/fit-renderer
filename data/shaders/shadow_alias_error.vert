//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;

//projection matrices
layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};
        
uniform mat4 in_ModelViewMatrix;

out vec4 o_vertex;
out vec3 o_normal;

void main()
{
    o_vertex = vec4(in_Vertex,1.0);
	o_normal = in_Normal;

    gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * vec4(in_Vertex, 1.0);
}
