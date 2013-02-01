#include "ShaderFeature.h"

ShaderFeature::ShaderFeature()
{
}

ShaderFeature::~ShaderFeature()
{
}

void ShaderFeature::AddVariable( string _name, string _type, int _varType )
{
	if( m_variables.find(_name) != m_variables.end() )
	{
		cerr<<"WARNING (AddVariable): variable with name "<<_name<<" already exist.\n";
		return;
	}

	Variable v;
	v.type = _type;
	v.name = (_varType==INOUT?"io_":"")+_name;
	v.varType = _varType;

	m_variables[_name] = v;
}

void ShaderFeature::ModifyVariable( string _name, string _opperation, string _opperand, int _shaderType )
{
	if( m_variables.find(_name) == m_variables.end() )
	{
		cerr<<"WARNING (ModifyVariable): no variable with name "<<_name<<"\n";
		return;
	}
	
	Modifier m;
	m.value = SG_TAB + m_variables[_name].name + _opperation + _opperand + SG_SEMINL;
	m.shType = _shaderType;

	m_modifiers.push_back( m );
}

vector<GLuint> & ShaderFeature::GetTextures()
{
	return m_textures;
}

string ShaderFeature::GetVars( int _shaderType )
{
	string output;
	string ioVars;
	string uniforms;

	for( m_iv = m_variables.begin(); m_iv != m_variables.end(); ++m_iv )
	{
		Variable v = m_iv->second;

		if( v.varType == INOUT )
			ioVars += InOut[_shaderType] + v.type + v.name + SG_SEMINL;
		else if( v.varType == VS_UNIFORM )
			uniforms += SG_UNIFORM + v.type + v.name + SG_SEMINL;
	}

	output = ioVars + SG_NEWLINE + uniforms;

	return output;
}

string ShaderFeature::GetModifiers( int _shaderType )
{
	string output;

	for( m_im = m_modifiers.begin(); m_im != m_modifiers.end(); ++m_im )
	{
		Modifier m = *m_im;

		if( _shaderType == m.shType )
			output += m.value;
	}

	return output;
}