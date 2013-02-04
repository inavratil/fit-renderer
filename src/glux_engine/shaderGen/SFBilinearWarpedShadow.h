#ifndef _SFBILINEARWARPEDSHADOW_H_
#define _SFBILINEARWARPEDSHADOW_H_

#include "ShaderFeature.h"

class SFBilinearWarpedShadow 
	: public ShaderFeature
{
protected:

public:
	SFBilinearWarpedShadow();
	~SFBilinearWarpedShadow();

	virtual void Init();

	virtual string GetVars( int _shaderType );
	virtual string GetModifiers( int _shaderType );
	virtual string GetFunc( int _shaderType );
};

#endif
