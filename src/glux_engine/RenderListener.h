/**
Class RenderListener
====================

Abstraktni trida (rozhrani) pro dalsi tridy, ktere "umi" neco rendrovat.
Resp. umi zpracovat anebo nastavit jednotlive rendrovaci "passy".

*/
#ifndef _RENDERLISTENER_H_
#define _RENDERLISTENER_H_

class TScene;

class RenderListener
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	TScene*		m_scene;

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
};

#endif

