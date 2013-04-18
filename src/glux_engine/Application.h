#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "globals.h"
#include "scene.h"



class Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	ScenePtr		m_scene;
	TwBar*			m_gui;

	//-- settings
	string	m_title;
	int		m_window_width;
	int		m_window_height;
	bool	m_is_fullscreen;
	bool	m_is_msaa_enabled;
	bool	m_is_gui_enabled;
	int		m_fps;
	int		m_memory_usage;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Application(void);
	virtual ~Application(void);

	int Run();
	
	void InitGLWindow();
	void ShowConfigDialog();
	void MainLoop();

	virtual void CreateContent() = 0;	// <== This must be filled with programmer
	virtual void InitGUI();

	void RenderScene();

	void MouseClicked(SDL_Event event);
	void MouseMoved(SDL_Event event);
	void KeyPressed(SDLKey key);

//-----------------------------------------------------------------------------
//-- Private methods

private: 
	void _Destroy();


};

#endif

