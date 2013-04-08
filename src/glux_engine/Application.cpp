#include "Application.h"

Application::Application(void)
{
}


Application::~Application(void)
{
}

int Application::Run()
{
	_Destroy();

	return 0;
}

void Application::_Destroy()
{
	//-- Terminate GUI
	TwTerminate();
	//-- Terminate SDL
    SDL_Quit();
}
