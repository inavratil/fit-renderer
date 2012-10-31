//generic vertex attributes
layout(location = 0) in vec3 in_Vertex;
layout(location = 2) in vec2 in_Coord;

//projection matrices
layout(std140) uniform Matrices{
  mat4 in_ProjectionMatrix;
};
        
uniform mat4 in_ModelViewMatrix;
uniform mat4 lightViewMatrix;
uniform vec2 near_far;

out vec2 fragTexCoord;
out vec4 cam_position, light_position, o_vertex;

void main()
{
    fragTexCoord = in_Coord;
	
	o_vertex = vec4(in_Vertex, 1.0);

	// camera screen position
    cam_position = in_ProjectionMatrix * in_ModelViewMatrix * o_vertex;

	// light screen position
	vec4 vertexEyeSpace = lightViewMatrix * o_vertex;
    gl_ClipDistance[0] = -vertexEyeSpace.z;
    
    float Length = length( vertexEyeSpace.xyz );
    vertexEyeSpace.z *= -1.0;
    vertexEyeSpace.xyz = normalize( vertexEyeSpace.xyz );

    vertexEyeSpace.z += 1.0;
    vertexEyeSpace.xy /= vertexEyeSpace.z;


    vertexEyeSpace.z = (Length - near_far.x)/(near_far.y  - near_far.x);
    vertexEyeSpace.w = 1.0; 

    light_position = vertexEyeSpace;

    gl_Position = cam_position;
}
