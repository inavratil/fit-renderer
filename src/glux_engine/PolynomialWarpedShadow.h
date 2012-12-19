#ifndef _POLYNOMIALWARPEDSHADOW_H_
#define _POLYNOMIALWARPEDSHADOW_H_

#include "IShadowTechnique.h"

class PolynomialWarpedShadow :
	public IShadowTechnique
{
public:
	PolynomialWarpedShadow();
	~PolynomialWarpedShadow(void);

	virtual bool Initialize(TLight* _light);
	virtual void PreRender();
	virtual void PostRender();
};

#endif
