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
	//AddVariable
	//"uniform float " + m_it_textures->first + "_intensity;\n"
	//"uniform sampler2DArray " + m_it_textures->first + ";\n";
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
			"uniform float tex_shadow_map_intensity;\n"
			"uniform sampler2DArray tex_shadow_map;\n";
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
			"  color *= ShadowOMNI(tex_shadow_map, tex_shadow_map_intensity);\n";
	}

	return output;
}

//-----------------------------------------------------------------------------

string SFDPShadowMap::GetFunc( int _shaderType )
{
	string output = ShaderFeature::GetFunc( _shaderType );

	output += LoadFunc("shadow_cut");

	return output;
}

//-----------------------------------------------------------------------------

