#ifndef _PASS_H_
#define _PASS_H_

#include "resources/FBOManager.h"

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

class Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	FBOManagerPtr			m_FBOManager;
	//FIXME: tohle by mely byt pointer na Shader a ne string
	string					m_shader;
	vector<PassTexture>		m_output_textures;
	GLuint					m_fbo;

	bool					m_activated;
	int						m_max_attachments;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Pass( GLuint _tex );
	virtual ~Pass(void);
	
//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Render();
	virtual void Activate();
	virtual void Deactivate();

//-----------------------------------------------------------------------------
	virtual void Process();

	void AttachOutputTexture( unsigned _pos, GLuint _tex );

	//-- Set/Get FBO Manager
	void SetFBOManager( FBOManagerPtr _fboManager ){ m_FBOManager = _fboManager; }
	FBOManagerPtr GetFBOManager(){ return m_FBOManager; }

	//-- Set/Get shader
	void SetShader( string _name ){ m_shader = _name; }
	string GetShader(){ return m_shader; }

	//-- Get output texture id
	GLuint GetTexture( unsigned _pos ){ return m_output_textures[_pos].id; }

};

typedef Pass* PassPtr;

#endif

