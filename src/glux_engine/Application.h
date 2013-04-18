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

	static const int g_cam_type = FPS;
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Application(void);
	virtual ~Application(void);

	int Run();
	
	void InitGLWindow();
	void InitScene();
	void ShowConfigDialog();
	void MainLoop();

	virtual void CreateContent( ScenePtr s ) = 0;	// <== This must be filled with programmer
	virtual void InitGUI();
	
	virtual void MouseClicked(SDL_Event event);
	virtual void MouseMoved(SDL_Event event);
	virtual void KeyPressed(SDLKey key);

	void RenderScene();

	

//-----------------------------------------------------------------------------
//-- Private methods

private: 
	void _Destroy();


};

#endif

