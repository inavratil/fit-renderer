//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;

//projection matrices
layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};
        
uniform mat4 in_ModelViewMatrix;

out vec4 o_vertex;

void main()
{
    o_vertex = vec4(in_Vertex,1.0);
    gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * vec4(in_Vertex, 1.0);
}
