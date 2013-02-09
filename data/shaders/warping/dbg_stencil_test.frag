// Aliaserr.frag - multires
in vec2 fragTexCoord;
in vec4 o_vertex;
in vec4 light_vertex;

out vec4 out_fragColor;

uniform mat4 cam_mv, cam_proj;
uniform mat4 lightMatrix;

bool IsInsideFrustum( vec2 _screenPos )
{
	return (lessThan(_screenPos.xy, vec2(1.0) ) == bvec2(1.0) && greaterThan(_screenPos.xy, vec2(0.0) )== bvec2(1.0));
}

void main() 
{
    vec4 cam_coords, cam_eye;
	float w;

	cam_eye = cam_mv * o_vertex;
    cam_coords = cam_proj * cam_eye;
	w = cam_coords.w;
    cam_coords = cam_coords/cam_coords.w;
    cam_coords.xy = cam_coords.xy * 0.5 + 0.5;

	if(w < 0.0 || !IsInsideFrustum(cam_coords.xy) )
		discard;

    out_fragColor = vec4( cam_coords.xy, 0.0, 1.0 );
}
