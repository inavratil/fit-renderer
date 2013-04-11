#ifndef _SIMPLEPASS_H_
#define _SIMPLEPASS_H_

#include "Pass.h"


struct PassTexture
{
	unsigned	pos;
	GLuint		id;
	PassTexture( unsigned _pos, GLuint _id )
	{
		pos = _pos;
		id = _id;
	}
};

class SimplePass : public Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	//FIXME: tohle by mely byt pointer na Shader a ne string
	string					m_shader;
	vector<PassTexture>		m_output_textures;
	FBOPtr					m_fbo;

	bool					m_activated;
	int						m_max_attachments;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	SimplePass( FBOManagerPtr _fbo_manager, unsigned _width, unsigned _height  );
	virtual ~SimplePass(void);
	
//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Render();
	virtual void Activate();
	virtual void Deactivate();

//-----------------------------------------------------------------------------

	void AttachOutputTexture( unsigned _pos, GLuint _tex );

	//-- Set/Get shader
	void SetShader( string _name ){ m_shader = _name; }
	string GetShader(){ return m_shader; }

	//-- Get output texture id
	GLuint GetTexture( unsigned _pos ){ return m_output_textures[_pos].id; }

//-----------------------------------------------------------------------------
private:
	void _Init( unsigned _width, unsigned _height );

};

#endif

