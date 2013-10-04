#ifndef _OMNISHADOW_H_
#define _OMNISHADOW_H_

#include "IShadowTechnique.h"

class OmniShadow : public IShadowTechnique
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	OmniShadow( TScene* _scene );
	virtual ~OmniShadow(void);

	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

//-----------------------------------------------------------------------------
//-- Private methods

private:
	void _EvaluateBestConfiguration();
};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

