#include "SFPolynomialWarpedShadow.h"

SFPolynomialWarpedShadow::SFPolynomialWarpedShadow()
{
	Init();
}

SFPolynomialWarpedShadow::~SFPolynomialWarpedShadow()
{
}

void SFPolynomialWarpedShadow::Init()
{
	AddVariable("ObjSpacePosition", SG_VEC4, ShaderFeature::INOUT);
	ModifyVariable("ObjSpacePosition", SG_ASSIGN, SG_V4_VERTCOORD, ShaderFeature::VS);	//vertex object-space position
}

string SFPolynomialWarpedShadow::GetVars( int _shaderType )
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
			"\n"
			"uniform mat4 coeffsX;\n"
			"uniform mat4 coeffsY;\n";
	}

	return output;
}

string SFPolynomialWarpedShadow::GetModifiers( int _shaderType )
{
	return ShaderFeature::GetModifiers( _shaderType );
}
