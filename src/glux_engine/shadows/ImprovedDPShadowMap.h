#ifndef _IMPROVEDDPSHADOWMAP_H_
#define _IMPROVEDDPSHADOWMAP_H_

#include "globals.h"
#include "DPShadowMap.h"

class ImprovedDPShadowMap : public DPShadowMap
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	ImprovedDPShadowMap( TScene* _scene );
	virtual ~ImprovedDPShadowMap(void);

	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

//-----------------------------------------------------------------------------
//-- Protected methods

protected:
	virtual void _PreDrawDepthMap();
	virtual glm::mat4 _GetLightViewMatrix( int _i );
	virtual void _UpdateShaderUniforms( int _i );

//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

