#ifndef _IPSMAPP_H_
#define _IPSMAPP_H_

#include "globals.h"
#include "glux_engine/Application.h"

enum IPSMTextures { OUTPUT, PING, PONG, MASK, CAM_ERR };

class IPSMApp : public Application
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

	int				m_scene_id;
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	IPSMApp(void);
	virtual ~IPSMApp(void);

	int SetupExperiments( const string& _filename );

	virtual void InitGUI();
	virtual void CreateContent();	// <== Must be defined
	virtual void UpdateScene();

	
	virtual void MouseMoved(SDL_Event event);
	virtual void KeyPressed( SDLKey _key );

//-----------------------------------------------------------------------------
//-- Private methods

private: 


};

#endif

