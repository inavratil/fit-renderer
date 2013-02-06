#ifndef _SFSPLINEWARPEDSHADOW_H_
#define _SFSPLINEWARPEDSHADOW_H_

#include "ShaderFeature.h"

class SFSplineWarpedShadow 
	: public ShaderFeature
{
protected:

public:
	SFSplineWarpedShadow();
	~SFSplineWarpedShadow();

	virtual void Init();

	virtual string GetVars( int _shaderType );
	virtual string GetModifiers( int _shaderType );
	virtual string GetFunc( int _shaderType );
};

#endif
