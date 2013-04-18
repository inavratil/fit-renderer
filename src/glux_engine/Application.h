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

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Application(void);
	virtual ~Application(void);

	int Run();

	void InitSDL();
	void ShowConfigDialog();

//-----------------------------------------------------------------------------
//-- Private methods

private: 
	void _Destroy();


};

#endif

