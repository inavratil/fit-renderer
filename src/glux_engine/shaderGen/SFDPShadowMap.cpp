#include "SFDPShadowMap.h"

//-----------------------------------------------------------------------------

SFDPShadowMap::SFDPShadowMap(void)
{
	Init();
}

//-----------------------------------------------------------------------------

SFDPShadowMap::~SFDPShadowMap(void)
{
}

//-----------------------------------------------------------------------------

void SFDPShadowMap::Init()
{
	AddVariable( "ObjSpacePosition", SG_VEC4, ShaderFeature::INOUT );
	ModifyVariable("ObjSpacePosition", SG_ASSIGN, SG_V4_VERTCOORD, ShaderFeature::VS);	//vertex object-space position
}

//-----------------------------------------------------------------------------

string SFDPShadowMap::GetVars( int _shaderType )
{
	string output = ShaderFeature::GetVars( _shaderType );;

	if( _shaderType == ShaderFeature::VS )
	{
	}
	else if( _shaderType == ShaderFeature::FS )
	{
		output +=
			"uniform float tex_shadowmap_intensity;\n"
			"uniform sampler2DArray tex_shadowmap;\n"
			"\n"
			"uniform vec2 near_far; // near and far plane for cm-cams\n"
			"uniform mat4 lightModelView[2]; //model view matrices for front and back side of paraboloid\n"
			"\n";
	}

	return output;

}

//-----------------------------------------------------------------------------

string SFDPShadowMap::GetModifiers( int _shaderType )
{
	string output = ShaderFeature::GetModifiers( _shaderType );

	if( _shaderType == ShaderFeature::VS )
	{
	}
	else if( _shaderType == ShaderFeature::FS )
	{
		output += "\n  //Shadow map projection\n"
			"  color *= ShadowOMNI( tex_shadowmap, tex_shadowmap_intensity);\n";
	}

	return output;
}

//-----------------------------------------------------------------------------

string SFDPShadowMap::GetFunc( int _shaderType )
{
	string output = ShaderFeature::GetFunc( _shaderType );

	if( _shaderType == ShaderFeature::VS )
	{
	}
	else if( _shaderType == ShaderFeature::FS )
	{
		output += LoadFunc("default_shadow_omni");
	}

	return output;
}

//-----------------------------------------------------------------------------

