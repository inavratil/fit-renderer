#ifndef _SFDPSHADOWMAP_H_
#define _SFDPSHADOWMAP_H_

#include "ShaderFeature.h"

class SFDPShadowMap : public ShaderFeature
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	SFDPShadowMap(void);
	virtual ~SFDPShadowMap(void);

//-----------------------------------------------------------------------------
//-- Virtual Methods	
	virtual void Init() = 0;
	virtual string GetVars( int _shaderType );
	virtual string GetModifiers( int _shaderType );
	virtual string GetFunc( int _shaderType );

};

#endif

