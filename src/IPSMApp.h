#ifndef _IPSMAPP_H_
#define _IPSMAPP_H_

#include "globals.h"
#include "glux_engine/Application.h"

class IPSMApp : public Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	glm::vec2 cut_angle;
	bool drawSM;
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	IPSMApp(void);
	virtual ~IPSMApp(void);

	virtual void CreateContent( ScenePtr s );	// <== Must be defined
	virtual void InitGUI();
	virtual void MouseMoved(SDL_Event event);
	virtual void KeyPressed(SDLKey key);

//-----------------------------------------------------------------------------
//-- Private methods

private: 


};

#endif

