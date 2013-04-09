#ifndef _PASS_H_
#define _PASS_H_

#include "resources/FBOManager.h"

class Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	FBOManagerPtr		m_FBOManager;
	//FIXME: tohle by mely byt pointer na Shader a ne string
	string				m_shader;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Pass(void);
	virtual ~Pass(void);
	
//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Render() = 0;

//-----------------------------------------------------------------------------
	//-- Set/Get FBO Manager
	void SetFBOManager( FBOManagerPtr _fboManager ){ m_FBOManager = _fboManager; }
	FBOManagerPtr GetFBOManager(){ return m_FBOManager; }

	//-- Set/Get shader
	void SetShader( string _name ){ m_shader = _name; }
	string GetShader(){ return m_shader; }

};

typedef Pass* PassPtr;

#endif

