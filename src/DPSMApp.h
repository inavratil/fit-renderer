#ifndef _DPSMAPP_H_
#define _DPSMAPP_H_

#include "sdk/Application.h"
#include "shadows/ImprovedDPShadowMap.h"

class DPSMApp : public Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	bool			m_param_is_drawSM_enabled;
	bool			m_param_is_draw_error_enabled;

	DPShadowMap*		m_shadow_technique;
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	DPSMApp(void);
	virtual ~DPSMApp(void);

	/*virtual*/ void InitGUI();
	/*virtual*/ void CreateContent();	// <== Must be redefined
	/*virtual*/ void UpdateScene();

//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

