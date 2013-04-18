#include "Application.h"
#include "ConfigDialog.h"

//-----------------------------------------------------------------------------

Application::Application(void) :
	m_scene( 0 ),
	m_gui( 0 ),
	m_window_width( 0 ),
	m_window_height( 0 ),
	m_is_fullscreen( false ),
	m_is_msaa_enabled( false ),
	m_is_gui_enabled( true ),
	m_fps( 0 ),
	m_memory_usage( 0 ),
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

		//InitGUI();
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

void Application::InitGUI()
{
	TwInit(TW_OPENGL, NULL);    
	TwWindowSize( m_window_width, m_window_height );
    m_gui = TwNewBar("Configuration");

    //scene information
    TwAddVarRO( m_gui, "fps", TW_TYPE_INT32, &m_fps, 
               " label='FPS'");
    /* TODO: bug in AntTweakBar?
    string gfx = (const char*)glGetString(GL_RENDERER);
    TwAddVarRO(ui, "gfx", TW_TYPE_STDSTRING, &gfx, 
               " label='GFX' group='Scene' ");
    */
    TwAddVarRO (m_gui, "mem_use", TW_TYPE_INT32, &m_memory_usage, 
               " label='Memory (MB)' group='Scene' ");
	TwAddVarRO( m_gui, "resx", TW_TYPE_INT32, &m_window_width, 
               " label='Width' group='Scene' ");
    TwAddVarRO( m_gui, "resy", TW_TYPE_INT32, &m_window_height, 
               " label='Height' group='Scene' ");
	TwAddVarRO( m_gui, "msaa", TW_TYPE_INT32, &m_is_msaa_enabled, 
               " label='Antialiasing' group='Scene' ");

    TwAddSeparator( m_gui, NULL, "group='Scene'");
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
	//Main loop
    SDLKey key = SDLK_UNKNOWN;
    SDL_Event event;
	bool keypress = false;    

	float anim = 0.0;
	int cycle;
    int time_now, time_nextMS, time_nextS, last_keypress = 0;
    time_now = time_nextMS = time_nextS = SDL_GetTicks();

    while(true)
    {
        //measure time
		time_now = SDL_GetTicks();

        if(time_now >= time_nextMS) //increment animation after 10ms
        {
            anim += 0.01f;
            time_nextMS += 10;
        }
        if(time_now >= time_nextS) //increment time after one second
        {
            time_nextS += 1000;
            //save FPS
            m_fps = cycle;
            cycle = 0;
        }

        //call drawing functions
        RenderScene();
		cycle++;
        if(m_is_gui_enabled)
            TwDraw();
        SDL_GL_SwapBuffers();

        //handle events with SDL
        if(SDL_PollEvent(&event))
        {
            int handled = TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
            if(!handled)
            {
                if(event.type == SDL_QUIT)
                    break;
                //keyboard events
                else if(event.type == SDL_KEYDOWN)
                {
                    keypress = true;
                    last_keypress = time_now;
                    key = event.key.keysym.sym;		//store pressed key
                }
                else if(event.type == SDL_KEYUP)
                    keypress = false;
                //mouse events
                else if(event.type == SDL_MOUSEMOTION)
                {
                    MouseMoved(event);
                    MouseClicked(event);
                }
            }
            else    //update values bound with tweak bar
            {
                //if(event.type != SDL_MOUSEMOTION)
                    //UpdateTweakBar();
            }
        }

        //call keyboard handle when key pressed
        if(keypress && (time_now - last_keypress > 150 || last_keypress == time_now) ) 
            KeyPressed(key);


		//meminfo
		if(GLEW_ATI_meminfo)
		{
			int meminfo[4];
			glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, meminfo);
			m_memory_usage = 1024 - meminfo[0]/1024;
		}
		else if( GLEW_NVX_gpu_memory_info )
		{
			int meminfo[4];
			glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &meminfo[0]);
			glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX , &meminfo[1]);
			glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX , &meminfo[2]);
			m_memory_usage = (meminfo[0] - meminfo[1])/1024;
		}
    }
}

//-----------------------------------------------------------------------------

void Application::RenderScene()
{
}

//-----------------------------------------------------------------------------

void Application::MouseClicked(SDL_Event event)
{
}

//-----------------------------------------------------------------------------

void Application::MouseMoved(SDL_Event event)
{
}

//-----------------------------------------------------------------------------

void Application::KeyPressed(SDLKey key)
{
}

//-----------------------------------------------------------------------------