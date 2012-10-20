//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Coord;
        
uniform mat4 in_ModelViewMatrix;
uniform mat4 in_CutMatrix;
uniform vec2 near_far; // near and far plane for cm-cams

uniform float ZOOM;

out vec2 fragTexCoord;
out vec4 position;

#ifdef PARABOLA_CUT
uniform vec4 cut_params; //-- min/max values for cut (on x-axis)
#endif

#define POLY_OFFSET 100.0

void main(void)
{
    fragTexCoord = in_Coord.xy;

    vec4 vertexEyeSpace = in_ModelViewMatrix * vec4(in_Vertex,1.0);
    gl_ClipDistance[0] = -vertexEyeSpace.z;

    vertexEyeSpace = in_CutMatrix * vertexEyeSpace;
    
    float Length = length( vertexEyeSpace.xyz );
    vertexEyeSpace.z *= -1.0;
    vertexEyeSpace.xyz = normalize( vertexEyeSpace.xyz );

    vertexEyeSpace.z += 1.0;
    vertexEyeSpace.xy /= vertexEyeSpace.z;

#ifdef PARABOLA_CUT
    vertexEyeSpace.x = ((vertexEyeSpace.x - cut_params.x)/(cut_params.y - cut_params.x)) * 2.0 - 1.0;
    vertexEyeSpace.y = ((vertexEyeSpace.y - cut_params.z)/(cut_params.w - cut_params.z)) * 2.0 - 1.0;
#else
    vertexEyeSpace.xy /= ZOOM;   //zoom-in
#endif

    vertexEyeSpace.z = (Length - near_far.x)/(near_far.y  - near_far.x);
    vertexEyeSpace.w = 1.0; 

    position = vertexEyeSpace;

    gl_Position = vertexEyeSpace; //ftransform();
}
