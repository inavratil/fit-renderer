//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;
        
uniform mat4 in_ModelViewMatrix;
uniform vec2 near_far; // near and far plane for cm-cams

uniform float ZOOM;

out vec2 fragTexCoord;
out vec4 position;
#define POLY_OFFSET 100.0

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

    vertexEyeSpace.xy /= ZOOM;   //zoom-in

    vertexEyeSpace.z = (Length - near_far.x)/(near_far.y  - near_far.x);
    vertexEyeSpace.w = 1.0; 

    position = vertexEyeSpace;

    gl_Position = vertexEyeSpace; //ftransform();
}
