#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

#include "sdk/RenderListener.h"
#include "scene.h"

class IShadowTechnique : public RenderListener
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	IShadowTechnique( TScene* _scene );
	virtual ~IShadowTechnique(void);

	//-- MUST DEFINE THESE METHODS IN DERIVED CLASS
	//virtual bool Initialize() = 0;
	//virtual void PreRender() = 0;
	//virtual void PostRender() = 0;

//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

