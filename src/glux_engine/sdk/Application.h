#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "globals.h"
#include "scene.h"
#include "ConfigDialog.h"
#include "StringUtil.h"

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
	int		m_is_msaa;
	bool	m_is_gui_enabled;
	bool	m_is_wireframe_enabled;
	int		m_fps;
	int		m_memory_usage;
	string	m_experiment_name;

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

	virtual void InitGUI();
	virtual void CreateContent() = 0;	// <== This must be filled with programmer
	virtual void UpdateScene();
	
	virtual void MouseClicked(SDL_Event event);
	virtual void MouseMoved(SDL_Event event);
	virtual void KeyInput( SDLKey _key, unsigned char _type );

	void RenderScene();

//-----------------------------------------------------------------------------
//-- Private methods

private: 
	void _Destroy();
};

#endif

