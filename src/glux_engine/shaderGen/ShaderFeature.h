#ifndef _SHADERFEATURE_H_
#define _SHADERFEATURE_H_

#include "../globals.h"

#include "shader_generator.h"



struct Variable {
	string type;
	string name;
	int varType;
};

struct Modifier {
	string value;
	int shType;
};

//FIXME: takto zpracovane textury jsou divne a asi to bude nutne predelat
struct FeatureTexure {
	GLuint id;
	float intensity;
	int shType;
};

const string InOut[] = { SG_IN, SG_OUT };

class ShaderFeature
{
public:
	enum SF_VariableTypes { VS_UNIFORM, FS_UNIFORM, INOUT };
	enum SF_ShaderType { FS, VS };

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	map<string,Variable>::iterator m_iv;
	map<string,Variable>	m_variables;

	vector<Modifier>::iterator m_im;
	vector<Modifier>			m_modifiers;

	//FIXME: Tohle je taky divne
	map<string,FeatureTexure>::iterator m_it;
	map<string,FeatureTexure>			m_textures;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	ShaderFeature();
	~ShaderFeature();

//-----------------------------------------------------------------------------
//-- Virtual Methods	
	virtual void Init() = 0;
	virtual string GetVars( int _shaderType );
	virtual string GetModifiers( int _shaderType );
	virtual string GetFunc( int _shaderType );
	
//-----------------------------------------------------------------------------
public:

	void AddVariable( string _name, string _type, int _varType );
	void ModifyVariable( string _name, string _opperation, string _opperand, int _shaderType );

	void AddTexture( string _name, GLuint _id, float _intensity = 1.0, int _shaderType = FS );
	map<string,FeatureTexure> & GetTextures();
	void ActivateTextures( GLuint _shader, int& _unitId );
	void DeactivateTextures( GLuint _shader, int& _unitId );

};

#endif
