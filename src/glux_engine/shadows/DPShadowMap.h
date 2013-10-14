#ifndef _DPSHADOWMAP_H_
#define _DPSHADOWMAP_H_

#include "IShadowTechnique.h"

class DPShadowMap : public IShadowTechnique
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	DPShadowMap( TScene* _scene );
	virtual ~DPShadowMap(void);

	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

//-----------------------------------------------------------------------------
//-- Private methods

private:
	void _Init();
	void _Destroy();
	void _EvaluateBestConfiguration();
};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

