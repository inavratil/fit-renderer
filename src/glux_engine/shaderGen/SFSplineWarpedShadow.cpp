#include "SFSplineWarpedShadow.h"

SFSplineWarpedShadow::SFSplineWarpedShadow()
{
	Init();
}

SFSplineWarpedShadow::~SFSplineWarpedShadow()
{
}

void SFSplineWarpedShadow::Init()
{
	AddVariable("ObjSpacePosition", SG_VEC4, ShaderFeature::INOUT);
	ModifyVariable("ObjSpacePosition", SG_ASSIGN, SG_V4_VERTCOORD, ShaderFeature::VS);	//vertex object-space position
}

string SFSplineWarpedShadow::GetVars( int _shaderType )
{
	string output = ShaderFeature::GetVars( _shaderType );;

	if( _shaderType == ShaderFeature::VS )
	{
	}
	else if( _shaderType == ShaderFeature::FS )
	{
		output +=
			"uniform vec3 near_far_bias; // near and far plane for cm-cams\n"
			"uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid\n"
			"\n"
			"uniform vec4 range;\n"
			"uniform float grid_res;\n"
			"\n";
	}

	return output;
}

string SFSplineWarpedShadow::GetModifiers( int _shaderType )
{
	return ShaderFeature::GetModifiers( _shaderType );
}

string SFSplineWarpedShadow::GetFunc( int _shaderType )
{
	string output;

	output += LoadFunc("warp_spline");
	output += LoadFunc("shadow_omni");

	return output;
}
