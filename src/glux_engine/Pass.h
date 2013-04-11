#ifndef _PASS_H_
#define _PASS_H_

#include "resources/FBOManager.h"

class Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	FBOManagerPtr			m_FBOManager;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Pass( FBOManagerPtr _fbo_manager ){ m_FBOManager = _fbo_manager; };
	virtual ~Pass(void){};

//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Activate()		= 0;
	virtual void Deactivate()	= 0;
	virtual void Render()		= 0;

//-----------------------------------------------------------------------------
	void Process()
	{
		Activate();
		Render();
		Deactivate();
	}

	//-- Set/Get FBO Manager
	void SetFBOManager( FBOManagerPtr _fboManager ){ m_FBOManager = _fboManager; }
	FBOManagerPtr GetFBOManager(){ return m_FBOManager; }


};

typedef Pass* PassPtr;

#endif

