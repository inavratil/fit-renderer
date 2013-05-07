//-- generic vertex attributes
layout(location = 0) in vec3 in_Vertex;

//-- modelview matrix
uniform mat4 in_ModelViewMatrix;

//-- projection and shadow matrices
layout(std140) uniform Matrices
{
  mat4 in_ProjectionMatrix ;
};

//-- Output variables

out vec3 camera_space_position;
out vec4 o_vertex;

//-- Vertex shader functions

void main()
{
  o_vertex = vec4(in_Vertex,1.0);

  camera_space_position = vec3(in_ModelViewMatrix * o_vertex );   //eyeview vector

  vec4 viewPos = in_ModelViewMatrix * o_vertex ;       //vertex transform
  gl_Position = in_ProjectionMatrix * viewPos;      //projection transform
}

