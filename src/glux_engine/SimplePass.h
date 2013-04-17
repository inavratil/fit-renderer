#ifndef _SIMPLEPASS_H_
#define _SIMPLEPASS_H_

#include "globals.h"
#include "Pass.h"
#include "texture.h"


struct PassTexture
{
	unsigned	pos;
	TexturePtr	tex;
	bool		is_depth;
	PassTexture( unsigned _pos, TexturePtr _tex, bool _is_depth = false )
	{
		pos = _pos;
		tex = _tex;
		is_depth = _is_depth;
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

	bool					m_depthbuffer_used;
	bool					m_activated;
	int						m_max_attachments;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	SimplePass( unsigned _width, unsigned _height  );
	virtual ~SimplePass(void);
	
//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Render();
	virtual void Activate();
	virtual void Deactivate();

	virtual bool Validate();

//-----------------------------------------------------------------------------

	void AttachOutputTexture( unsigned _pos, TexturePtr _tex, bool _is_depth = false );
	void EnableDepthBuffer(){ m_depthbuffer_used = true; }
	void DisableDepthBuffer(){ m_depthbuffer_used = false; }

	//-- Set/Get shader
	void SetShader( string _name ){ m_shader = _name; }
	string GetShader(){ return m_shader; }

	//-- Get output texture id
	//GLuint GetTexture( unsigned _pos ){ return m_output_textures[_pos].id; }

	//-- Get FBO
	FBOPtr GetFBO(){ return m_fbo; }

//-----------------------------------------------------------------------------
private:
	void _Init( unsigned _width, unsigned _height );

};

typedef SimplePass* SimplePassPtr;

#endif

