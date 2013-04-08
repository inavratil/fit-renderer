#include "Application.h"

Application::Application(void)
{
}


Application::~Application(void)
{
}

int Application::Run()
{
	Destroy();

	return 0;
}

void Application::Destroy()
{
	//-- Terminate GUI
	TwTerminate();
	//-- Terminate SDL
    SDL_Quit();
}
