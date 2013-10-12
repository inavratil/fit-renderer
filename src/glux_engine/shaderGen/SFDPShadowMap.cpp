#include "SFDPShadowMap.h"

//-----------------------------------------------------------------------------

SFDPShadowMap::SFDPShadowMap(void)
{
}

//-----------------------------------------------------------------------------

SFDPShadowMap::~SFDPShadowMap(void)
{
}

//-----------------------------------------------------------------------------

void SFDPShadowMap::Init()
{
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
//			"uniform float " + m_it_textures->first + "_intensity;\n"
//			"uniform sampler2DArray " + m_it_textures->first + ";\n";
		"\n";
	}

	return output;

}

//-----------------------------------------------------------------------------

string SFDPShadowMap::GetModifiers( int _shaderType )
{
	return ShaderFeature::GetModifiers( _shaderType );
}

//-----------------------------------------------------------------------------

string SFDPShadowMap::GetFunc( int _shaderType )
{
	return ShaderFeature::GetFunc( _shaderType );
}

//-----------------------------------------------------------------------------

