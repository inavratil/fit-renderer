#ifndef _WARPDPSMAPP_H_
#define _WARPDPSMAPP_H_

#include "globals.h"
#include "sdk/Application.h"

#include "shadows/SplineWarpedShadow.h"

enum IPSMTextures { OUTPUT, PING, PONG, MASK, CAM_ERR };

class WarpDPSMApp : public Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	//-- parameters
	glm::vec2		m_param_cut_angle;
	bool			m_param_is_drawSM_enabled;
	bool			m_param_is_warping_enabled;
	bool			m_param_is_draw_error_enabled;
	int				m_param_preview_texture_id;

	SplineWarpedShadow*		m_shadow_technique;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	WarpDPSMApp(void);
	virtual ~WarpDPSMApp(void);

	int SetupExperiments( const string& _filename );

	/*virtual*/ void InitGUI();
	/*virtual*/ void CreateContent();	// <== Must be redefined
	/*virtual*/ void UpdateScene();

	
	/*virtual*/ void MouseMoved(SDL_Event event);
	/*virtual*/ void KeyInput( SDLKey _key, unsigned char _type );

//-----------------------------------------------------------------------------
//-- Private methods

private: 


};

#endif

