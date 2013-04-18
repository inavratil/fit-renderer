#include "Application.h"
#include "ConfigDialog.h"

//-----------------------------------------------------------------------------

Application::Application(void) :
	m_scene( 0 ),
	m_window_width( 0 ),
	m_window_height( 0 ),
	m_is_fullscreen( false ),
	m_is_msaa_enabled( false ),
	m_title( "Window" )
{
	//FIXME: tohle ma byt v Run!
	try
	{
		//initialize SDL video
		//SDL_putenv("SDL_VIDEO_WINDOW=center");
		if(SDL_Init(SDL_INIT_VIDEO) < 0) throw ERR;;
		ShowConfigDialog();
		InitGLWindow();

		/*
		m_scene = new TScene();
		if(!m_scene->PreInit( 
			m_window_width, m_window_height, 
			0.1f, 10000.0f, 45.0f,				// <== FIXME: Tohle musi prijit do kamery (nebo nekam)
			m_is_msaa_enabled, false, false
			)
			) throw ERR;
		*/
	}
	catch( int )
	{
		_Destroy();
		//return 1;
	}

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
	if( m_scene )
		delete m_scene;

	//-- Terminate GUI
	TwTerminate();
	//-- Terminate SDL
    SDL_Quit();
}

//-----------------------------------------------------------------------------

void Application::InitGLWindow()
{	
	if( m_window_width <= 0 && m_window_height <= 0 )
		return;

	//enable double buffering with a 24bit Z buffer.
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //enable multisampling
	if( m_is_msaa_enabled )
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_is_msaa_enabled);
	}

	SDL_WM_SetCaption( m_title.c_str(), 0);

    //set video mode
    int mode = SDL_OPENGL;
    if(	m_is_fullscreen	)
       mode |= SDL_FULLSCREEN;

    if( SDL_SetVideoMode(m_window_width, m_window_height, 32, mode) == NULL)
    {
        ShowMessage("Display mode not supported!\n");
		throw ERR;
    }

	SDL_WarpMouse((Uint16)m_window_width/2, (Uint16)m_window_height/2);

	/*
#ifndef _LINUX_
    //doesn't work/exist on a testing linux system. Exists in SDL 1.3
    //turn off vsync
    SDL_GL_SetSwapInterval(0);
#endif
*/
    //init GLEW
    if(glewInit() != GLEW_OK)
    {
        ShowMessage("GLEW initialization failed!\n");
        throw ERR;
    }
}

//-----------------------------------------------------------------------------

void Application::ShowConfigDialog()
{
	ConfigDialog* dialog = new ConfigDialog();
	//dialog->Display();

	m_window_width = dialog->GetInteger( "Window width" );
	m_window_height = dialog->GetInteger( "Window height" );
	m_is_fullscreen = dialog->GetBoolean( "Full screen" );
	m_is_msaa_enabled = dialog->GetBoolean( "" );
	m_title = dialog->GetString( "Title" );
	delete dialog;
}

//-----------------------------------------------------------------------------

void Application::MainLoop()
{
}

//-----------------------------------------------------------------------------
