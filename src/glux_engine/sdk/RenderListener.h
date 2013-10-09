/**
Class RenderListener
====================

Abstraktni trida (rozhrani) pro dalsi tridy, ktere "umi" neco rendrovat.
Resp. umi zpracovat anebo nastavit jednotlive rendrovaci "passy".

*/
#ifndef _RENDERLISTENER_H_
#define _RENDERLISTENER_H_

#include "sdk/Pass.h"

class TScene;

class RenderListener
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	TScene*		m_scene;
	
	//-- associative array of render passes
	map<string, PassPtr>				m_passes;
	map<string, PassPtr>::iterator		m_it_pass;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	RenderListener( TScene* _scene );
	virtual ~RenderListener(void);

//-----------------------------------------------------------------------------	
//-- Virtual methods

	virtual bool Initialize() = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

	void AppendPass( string _name, PassPtr _pass );
	PassPtr GetPassPtr( string _name );

//-----------------------------------------------------------------------------
//-- Private methods
private:
	void _Init();
	void _Destroy();
};

#endif

