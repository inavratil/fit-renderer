//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Coord;

//projection matrices
layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};
        
uniform mat4 in_ModelViewMatrix;
uniform vec2 near_far; // near and far plane for cm-cams

out vec2 fragTexCoord;
out vec3 eyeNorm;
out vec4 eyePos;
out vec4 o_vertex;

void main()
{
	vec4 vertex = vec4(in_Vertex, 1.0);
	eyePos = in_ModelViewMatrix * vertex;
	eyeNorm = mat3(in_ModelViewMatrix) * in_Normal;
    fragTexCoord = in_Coord;

    vec4 vertexEyeSpace = in_ModelViewMatrix * vec4(in_Vertex,1.0);
    gl_ClipDistance[0] = -vertexEyeSpace.z;
    
    float Length = length( vertexEyeSpace.xyz );
    vertexEyeSpace.z *= -1.0;
    vertexEyeSpace.xyz = normalize( vertexEyeSpace.xyz );

    vertexEyeSpace.z += 1.0;
    vertexEyeSpace.xy /= vertexEyeSpace.z;

    vertexEyeSpace.z = (Length - near_far.x)/(near_far.y  - near_far.x);
    vertexEyeSpace.w = 1.0; 

    gl_Position = vertexEyeSpace; //ftransform();
    //gl_Position = in_ProjectionMatrix * in_ModelViewMatrix * vertex;
}
