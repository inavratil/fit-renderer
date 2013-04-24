#include "Application.h"

//-----------------------------------------------------------------------------

Application::Application(void) :
	m_scene( 0 ),
	m_camera( 0 ),
	m_gui( 0 ),
	m_window_width( 0 ),
	m_window_height( 0 ),
	m_is_fullscreen( false ),
	m_is_msaa( 1 ),
	m_is_gui_enabled( true ),
	m_is_wireframe_enabled( false ),
	m_fps( 0 ),
	m_memory_usage( 0 ),
	m_title( "Window" ),
	m_experiment_name( "" )
{
}

//-----------------------------------------------------------------------------

Application::~Application(void)
{
}

//-----------------------------------------------------------------------------

int Application::Run()
{
	try
	{
		//initialize SDL video
		//SDL_putenv("SDL_VIDEO_WINDOW=center");
		if(SDL_Init(SDL_INIT_VIDEO) < 0) throw ERR;

		ShowConfigDialog();
		InitGLWindow();		
		InitScene();		
		InitGUI();

		MainLoop();
	}
	catch( int )
	{
		_Destroy();
		return 1;
	}

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
	if( m_is_msaa > 1)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_is_msaa);
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

	//bar settings
    TwDefine("Configuration refresh=0.1 ");

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
	TwAddVarRO( m_gui, "msaa", TW_TYPE_INT32, &m_is_msaa, 
               " label='Antialiasing' group='Scene' ");
	TwAddVarRW( m_gui, "wire", TW_TYPE_BOOLCPP, &m_is_wireframe_enabled, 
               " label='Wireframe' group='Scene' key=x");
}

//-----------------------------------------------------------------------------

void Application::InitScene()
{
	m_scene = new TScene();

	m_camera = m_scene->CreateCamera();
	m_camera->SetFOVy( 45.0f );
	m_camera->SetNearPlane( 0.1f );
	m_camera->SetFarPlane( 1000.0f );

	if( !m_scene->PreInit( m_window_width, m_window_height )	) throw ERR;


	CreateContent();

	if( !m_scene->PostInit() ) throw ERR;

	//-- update state of the scene according to application parameters
	UpdateScene();
}

//-----------------------------------------------------------------------------

void Application::ShowConfigDialog()
{
	ConfigDialog* dialog = new ConfigDialog();
	dialog->Load( "config.cfg" );

	//dialog->Display();

	string section( "System" );
	string option;
	//-- Mandatory properties
	if( (option = dialog->GetSetting( "Window width", section )).empty() ) throw ERR;
	m_window_width = StringUtil::ParseInt( option );
	if( (option = dialog->GetSetting( "Window height", section )).empty() ) throw ERR;
	m_window_height = StringUtil::ParseInt( option );

	//-- Auxiliary properties
	option = dialog->GetSetting( "Full screen", section, "No" );
	m_is_fullscreen = (option == "Yes" || option == "yes" );
	option = dialog->GetSetting( "MSAA", section, "1" );
	m_is_msaa = StringUtil::ParseInt( option );
	
	m_title = dialog->GetSetting( "Title", section );
	m_experiment_name = dialog->GetSetting( "Experiment", section );

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
	int cycle = 0;
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
					if( key == SDLK_ESCAPE )
						break;
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
				//FIXME: odtud se to bude muset presunout vys pred RenderScene, aby to mohlo reagovat i na jine klavesy nez z tweakbaru
                UpdateScene();
            }
        }

        //call keyboard handle when key pressed
        if(keypress && (time_now - last_keypress > 150 || last_keypress == time_now) ) 
            KeyPressed( key );
    }
}

//-----------------------------------------------------------------------------

void Application::RenderScene()
{
	m_scene->Redraw();
	
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

//-----------------------------------------------------------------------------

void Application::UpdateScene()
{
	m_scene	->	Wireframe	( m_is_wireframe_enabled );
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Handles mouse clicks
@param button pressed mouse button
@param state pouse button state
@param x X-coordinate of click
@param y Y-coordinate of click
****************************************************************************************************/
void Application::MouseClicked(SDL_Event event)
{
	int status = event.button.button;

	//hide cursor at moving
	if(status == SDL_BUTTON_LEFT)
		SDL_ShowCursor(SDL_DISABLE);
	else
		SDL_ShowCursor(SDL_ENABLE);
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Handles mouse motion
@param event of the mouse
****************************************************************************************************/
void Application::MouseMoved(SDL_Event event)
{
	int status = event.button.button;

	glm::vec3 rot = m_scene->GetCameraRot();
    glm::vec3 pos = m_scene->GetCameraPos();

	//camera movement
	if(g_cam_type == FPS && status == SDL_BUTTON_LEFT)  //FPS camera
	{
		rot.x += event.motion.yrel;     //set the xrot to xrot with the addition of the difference in the y position
		rot.y += event.motion.xrel;     //set the xrot to yrot with the addition of the difference in the x position
	}
	else    //orbiting camera
	{
		int x = event.motion.xrel;
		int y = event.motion.yrel;
		if(status == SDL_BUTTON_LEFT)   //rotate
		{
			m_scene->RotateCamera(float(y), A_X);
			m_scene->RotateCamera(float(x), A_Y);
		}
		else if(status == 4)  //zoom.  proc nefuguje??SDL_BUTTON_RIGHT
			m_scene->MoveCamera(0.0f, 0.0f, float(x));
		else if(status == SDL_BUTTON_MIDDLE)  //position
			m_scene->MoveCamera( float(x), -float(y), 0.0f);
	}

	//camera object rotation
	//s->RotateObjAbs("camera", rot.x, A_X);
	//s->RotateObjAbs("camera", -rot.y, A_Y);
	m_scene->RotateCameraAbs(rot.x, A_X);
	m_scene->RotateCameraAbs(rot.y, A_Y);
}

//-----------------------------------------------------------------------------

void Application::KeyPressed( SDLKey _key )
{
	const float INC = 5.0;
	glm::vec3 lpos1 = m_scene->GetLightPos(0);

    //camera rotation and position
    glm::vec3 rot = m_scene->GetCameraRot();
    glm::vec3 pos = m_scene->GetCameraPos();
	switch(_key)
    {  
	        //WSAD camera movement
    case SDLK_s:
        pos.x += INC*glm::sin( glm::radians( rot.y ) );
        pos.z -= INC*glm::cos( glm::radians( rot.y ) );
        pos.y -= INC*glm::sin( glm::radians( rot.x ) );
        break;
    case SDLK_w: 
        pos.x -= INC*glm::sin( glm::radians( rot.y ) );
        pos.z += INC*glm::cos( glm::radians( rot.y ) );
        pos.y += INC*glm::sin( glm::radians( rot.x ) );
        break;
    case SDLK_d:
        pos.x -= INC*glm::cos( glm::radians( rot.y ) );
        pos.z -= INC*glm::sin( glm::radians( rot.y ) );
        break;		 
    case SDLK_a:
        pos.x += INC*glm::cos( glm::radians( rot.y ) );
        pos.z += INC*glm::sin( glm::radians( rot.y ) );
        break;		 

        //main light movement
    case SDLK_i: lpos1.z -= INC; m_scene->MoveLight(0,lpos1); break;
    case SDLK_k: lpos1.z += INC; m_scene->MoveLight(0,lpos1); break;
    case SDLK_j: lpos1.x -= INC; m_scene->MoveLight(0,lpos1); break;
    case SDLK_l: lpos1.x += INC; m_scene->MoveLight(0,lpos1); break;
    case SDLK_u: lpos1.y += INC; m_scene->MoveLight(0,lpos1); break;
    case SDLK_o: lpos1.y -= INC; m_scene->MoveLight(0,lpos1); break;
	
    default:
        break;
    }

	//update FPS camera
    if(g_cam_type == FPS)
    {
        m_scene->MoveCameraAbs(pos.x, pos.y, pos.z);
        m_scene->RotateCameraAbs(rot.x, A_X);
        m_scene->RotateCameraAbs(rot.y, A_Y);
    }
	//cout<<"LIGHT: "<<lpos1.x<<","<<lpos1.y<<","<<lpos1.z<<endl;
    //s->PrintCamera();

    //camera object position
    //s->MoveObjAbs("camera", pos.x, pos.y, pos.z);
}

//-----------------------------------------------------------------------------