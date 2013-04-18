#include "Application.h"
#include "ConfigDialog.h"

static int resx = 1024, resy = 1024;

//-----------------------------------------------------------------------------

Application::Application(void) :
	m_scene( 0 )
{
	//FIXME: tohle ma byt v Run!
	InitSDL();
	//ShowConfigDialog();
}

//-----------------------------------------------------------------------------

Application::~Application(void)
{
}

//-----------------------------------------------------------------------------

int Application::Run()
{
	_Destroy();

	return 0;
}

//-----------------------------------------------------------------------------

void Application::_Destroy()
{
	delete m_scene;

	//-- Terminate GUI
	TwTerminate();
	//-- Terminate SDL
    SDL_Quit();
}

//-----------------------------------------------------------------------------

void Application::InitSDL()
{
	
    //initialize SDL video
    //SDL_putenv("SDL_VIDEO_WINDOW=center");
    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
        exit(1);

    //enable double buffering with a 24bit Z buffer.
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //enable multisampling
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);

    SDL_WM_SetCaption("FITRenderer v0.5", 0);

    //set video mode
    int mode = SDL_OPENGL;
    //if(fullscreen)
    //   mode |= SDL_FULLSCREEN;

    if(SDL_SetVideoMode(resx, resy, 32, mode) == NULL)
    {
        ShowMessage("Display mode not supported!\n");
        SDL_Quit();
        exit(1);
    }

	SDL_WarpMouse((Uint16)resx/2, (Uint16)resy/2);
}

//-----------------------------------------------------------------------------

void Application::ShowConfigDialog()
{
	ConfigDialog* dialog = new ConfigDialog();
	dialog->Display();
	delete dialog;
}

//-----------------------------------------------------------------------------