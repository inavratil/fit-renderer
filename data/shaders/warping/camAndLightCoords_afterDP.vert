//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;

//projection matrices
layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};        
uniform mat4 in_ModelViewMatrix;

uniform vec2 near_far; // near and far plane for cm-cams

out vec2 fragTexCoord;
out vec4 o_vertex;
out vec4 light_vertex;

#define POLY_OFFSET 100.0

const float SCREEN_X = 1024.0;
const float SCREEN_Y = 1024.0;

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

    o_vertex = vec4(in_Vertex,1.0);
	light_vertex = vertexEyeSpace;

    gl_Position = vertexEyeSpace; //ftransform();
    //gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * vec4(in_Vertex,1.0);
}
