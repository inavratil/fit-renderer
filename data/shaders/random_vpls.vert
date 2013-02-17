//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;

//projection matrices
layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};
        
uniform mat4 in_ModelViewMatrix;

void main()
{
    gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * vec4(in_Vertex, 1.0);
}
