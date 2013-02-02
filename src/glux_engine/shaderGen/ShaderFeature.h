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

const string InOut[] = { SG_IN, SG_OUT };

class ShaderFeature
{
public:
	enum SF_VariableTypes { VS_UNIFORM, FS_UNIFORM, INOUT };
	enum SF_ShaderType { FS, VS };
	
protected:
	map<string,Variable>::iterator m_iv;
	map<string,Variable>	m_variables;

	vector<Modifier>::iterator m_im;
	vector<Modifier>			m_modifiers;

	vector<GLuint>::iterator m_it;
	vector<GLuint>			m_textures;


public:
	ShaderFeature();
	~ShaderFeature();

	virtual void Init() = 0;

public:

	void AddVariable( string _name, string _type, int _varType );
	void ModifyVariable( string _name, string _opperation, string _opperand, int _shaderType );

	void AddTexture( GLuint _id, float _intensity );
	vector<GLuint> & GetTextures();

	virtual string GetVars( int _shaderType );
	virtual string GetModifiers( int _shaderType );
};

#endif
