#ifndef _SFPOLYNOMIALRWARPEDSHADOW_H_
#define _SFPOLYNOMIALRWARPEDSHADOW_H_

#include "ShaderFeature.h"

class SFPolynomialWarpedShadow 
	: public ShaderFeature
{
protected:

public:
	SFPolynomialWarpedShadow();
	~SFPolynomialWarpedShadow();

	virtual void Init();

	virtual string GetVars( int _shaderType );
	virtual string GetModifiers( int _shaderType );
};

#endif
