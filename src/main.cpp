#include "main_ui.h"


//*****************************************************************************
//Initialize OpenGL settings for scene
bool InitScene(int resx, int resy)
{ 
    s = new TScene();
    if(!s->PreInit(resx, resy, 0.1f, 10000.0f,45.0f, msaa, false, false)) 
        return false;

	try{
		const char *cubemap[] = {   "data/tex/cubemaps/posx.tga", "data/tex/cubemaps/negx.tga",
			"data/tex/cubemaps/posy.tga", "data/tex/cubemaps/negy.tga",
			"data/tex/cubemaps/posz.tga", "data/tex/cubemaps/negz.tga" };

		//skybox
		s->AddMaterial("mat_sky",white,white,white,0.0,0.0,0.0,NONE);
		s->AddTexture("mat_sky",cubemap);
		s->AddObject("sky",CUBE,5000.0,5000.0);
		s->SetMaterial("sky","mat_sky");


		pos = glm::vec3(-540, -43, 0);
		rot = glm::vec3(-20, 270, 0.0f);

		s->LoadScene("data/obj/scenes/sibenik.3ds");
		s->AddLight(0, dgrey, silver, grey, glm::vec3(0.0,50.0,0.0), 1000.0f);
		s->MoveLight(0, glm::vec3(-120, 350, 0));

#ifdef USE_DP        
		//dual-paraboloid shadow parameters        
		s->SetShadow(0, SHADOW_RES, OMNI, 0.3f, true);
		s->DPSetPCF(use_pcf);
		s->DPShadowMethod( dpshadow_method );         
		s->DPTessellation(dpshadow_tess);
		s->DPDrawSM(drawSM);
		s->RotateParaboloid(parab_rot);
#else
		s->SetShadow(0, SHADOW_RES, SPOT, 0.3f);
#endif

        
        //toggle effects (HDR & SSAO)
        //s->UseHDR();
        //s->UseSSAO();

        //prepare render to texture
        s->CreateHDRRenderTarget(-1, -1, GL_RGBA16F, GL_FLOAT);

        //cast/receive shadows
        s->CastShadow("sky",false);
        s->ReceiveShadow("mat_sky",false);

        ////General HDR settings
        //s->SetUniform("mat_tonemap","exposure",1.0);
        //s->SetUniform("mat_tonemap","bloomFactor",0.3);
        //s->SetUniform("mat_tonemap","brightMax",1.7);
        //s->SetUniform("mat_bloom_hdr_ssao","THRESHOLD",0.5);
        ////SSAO settings
        //s->SetUniform("mat_bloom_hdr_ssao","intensity",0.7);
        //s->SetUniform("mat_bloom_hdr_ssao","radius",0.004);
        //s->SetUniform("mat_bloom_hdr_ssao","bias",1.5);
        
        s->AddMaterial("show_depth");
        s->CustomShader("show_depth", "data/shaders/showDepth.vert", "data/shaders/showDepth.frag");
        s->AddMaterial("show_depth_omni");
        s->CustomShader("show_depth_omni", "data/shaders/showDepth.vert", "data/shaders/showDepth_omni.frag");
    }
    catch(int)
    {
        cerr<<"Initialization failed.";
        return false;
    }
    s->SetCamType(cam_type);
    return s->PostInit();
}

string name;
//*****************************************************************************
//Main drawing function
void Redraw()
{
    //update FPS camera
    if(cam_type == FPS)
    {
        s->MoveCameraAbs(pos.x, pos.y, pos.z);
        s->RotateCameraAbs(rot.x, A_X);
        s->RotateCameraAbs(rot.y, A_Y);
    }

    s->Redraw();
    cycle++;                          //increment draw counter 

    //update info
    dp_frontFOV = s->DPGetFOV();

    //meminfo (ATI only)
    if(GLEW_ATI_meminfo)
    {
        static int meminfo[4];
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, meminfo);
        mem_use = 1024 - meminfo[0]/1024;
    }
}

//*****************************************************************************
//Keyboard input
const float INC = 1.0;
void KeyInput(SDLKey key)
{
    glm::vec3 lpos1 = s->GetLightPos(0);

    //camera rotation and position
    glm::vec2 rotrad;
    rot = s->GetCameraRot();
    pos = s->GetCameraPos();

    switch(key)
    {     
           //WSAD camera movement
    case SDLK_s:
        rotrad.y = (rot.y / 180 * PI);
        rotrad.x = (rot.x / 180 * PI);
        pos.x += 5*INC*float(sin(rotrad.y));
        pos.z -= 5*INC*float(cos(rotrad.y));
        pos.y -= 5*INC*float(sin(rotrad.x));
        break;
    case SDLK_w: 
        rotrad.y = (rot.y / 180 * PI);
        rotrad.x = (rot.x / 180 * PI);
        pos.x -= 5*INC*float(sin(rotrad.y));
        pos.z += 5*INC*float(cos(rotrad.y));
        pos.y += 5*INC*float(sin(rotrad.x));
        break;
    case SDLK_d:
        rotrad.y = (rot.y / 180 * PI);
        pos.x -= 5*INC*float(cos(rotrad.y));
        pos.z -= 5*INC*float(sin(rotrad.y));
        break;
    case SDLK_a:
        rotrad.y = (rot.y / 180 * PI);
        pos.x += 5*INC*float(cos(rotrad.y));
        pos.z += 5*INC*float(sin(rotrad.y));
        break;

        //main light movement
    case SDLK_i: lpos1.z -= INC; s->MoveLight(0,lpos1); break;
    case SDLK_k: lpos1.z += INC; s->MoveLight(0,lpos1); break;
    case SDLK_j: lpos1.x -= INC; s->MoveLight(0,lpos1); break;
    case SDLK_l: lpos1.x += INC; s->MoveLight(0,lpos1); break;
    case SDLK_u: lpos1.y += INC; s->MoveLight(0,lpos1); break;
    case SDLK_o: lpos1.y -= INC; s->MoveLight(0,lpos1); break;


    case SDLK_ESCAPE:        //ESCAPE - quit
        delete s;
        SDL_Quit();
        exit(0);
        break;

    default:
        break;
    }
  
    //s->PrintCamera();

    //camera object position
    //s->MoveObjAbs("camera", pos.x, pos.y, pos.z);
}

/**
****************************************************************************************************
@brief Handles mouse clicks
@param button pressed mouse button
@param state pouse button state
@param x X-coordinate of click
@param y Y-coordinate of click
****************************************************************************************************/
int status;                                     //which button?
void MouseClick(SDL_Event event)
{ 
    status = event.button.button;

    //hide cursor at moving
    if(status == SDL_BUTTON_LEFT)
        SDL_ShowCursor(SDL_DISABLE);
    else
        SDL_ShowCursor(SDL_ENABLE);
}

/**
****************************************************************************************************
@brief Handles mouse motion
@param event of the mouse
****************************************************************************************************/
void MouseMotion(SDL_Event event)
{
    if(move_parab)
    {
        if(status == SDL_BUTTON_LEFT)
        {
            if( SDL_GetModState() & KMOD_LCTRL )
                parab_rot.y += 0;
            else
                parab_rot.y += event.motion.xrel;

            if( SDL_GetModState() & KMOD_LSHIFT )
                parab_rot.x += 0;
            else
                parab_rot.x += event.motion.yrel; 
            s->RotateParaboloid(parab_rot);

        }
    }
    //camera movement
    else
    {
        //camera movement
        if(cam_type == FPS && status == SDL_BUTTON_LEFT)  //FPS camera
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
                s->RotateCamera(float(y), A_X);
                s->RotateCamera(float(x), A_Y);
            }
            else if(status == 4)  //zoom.  proc nefuguje??SDL_BUTTON_RIGHT
                s->MoveCamera(0.0f, 0.0f, float(x));
            else if(status == SDL_BUTTON_MIDDLE)  //position
                s->MoveCamera( float(x), -float(y), 0.0f);
        }

        //camera object rotation
        //s->RotateObjAbs("camera", rot.x, A_X);
        //s->RotateObjAbs("camera", -rot.y, A_Y);
    }
}

/**
****************************************************************************************************
@brief Print program usage if wrong parameters have been used. Then exits program
****************************************************************************************************/
void WrongParams()
{
    cout<<"Wrong parameters.\n"
        "Usage: gluxEngine.exe [-w|-f resX resY][-aa value]\n"
        "Parameters:\n"
        "-w,-f: windowed/fullscreen mode\n"
        "resX, resY: screen resolution in pixels\n"
        "-aa: antialiasing strength (0,1 = off)\n";
    exit(1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//********************************* MAIN ********************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    string param;

    //parse parameters
    for(int i=1; i<argc; i++)
    {
        param = argv[i];
        ////////////////////////////////////////////////
        //fullscreen/windowed mode
        if(param == "-w" || param == "-f")
        {
            if(param == "-w")
                fullscreen = false;
            else if(param == "-f")
                fullscreen = true;
            //display resolution
            if(i+2 < argc)
            {
                resx = atoi(argv[i+1]);
                resy = atoi(argv[i+2]);
                i += 2;
            }
            else
                WrongParams();
        }
        ///////////////////////////////////////////////
        //antialiasing
        else if(param == "-aa")
        {
            if(i+1 < argc)
            {
                msaa = atoi(argv[i+1]);
                i++;
            }
            else
                WrongParams();
        }
        else if(param == "-dpsm")
            dpshadow_method = DPSM;

        ///////////////////////////////////////////
        //use PCF
        else if(param == "-pcf")
            use_pcf = true;
        //////////////////////////////////////////
        //use tessellation
        else if(param == "-tess")
            dpshadow_tess = true;
        //////////////////////////////////////////
        //draw tweak bar
        else if(param == "-no_ui")
            draw_ui = false;

        ///////////////////////////////////////////
        //error
        else
            WrongParams();
    }

    //initialize SDL video
    //SDL_putenv("SDL_VIDEO_CENTERED=center");
    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
        exit(1);

    //enable double buffering with a 24bit Z buffer.
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //enable multisampling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);

    SDL_WM_SetCaption("FITRenderer v0.0", 0);

    //set video mode
    int mode = SDL_OPENGL;
    if(fullscreen)
        mode |= SDL_FULLSCREEN;

    if(SDL_SetVideoMode(resx, resy, 32, mode) == NULL)
    {
        ShowMessage("Display mode not supported!\n");
        SDL_Quit();
        return 1;
    }
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
        SDL_Quit();
        return 1;
    }

    //init scene
    if(!InitScene(resx,resy)) exit(1);

    SDL_WarpMouse((Uint16)resx/2, (Uint16)resy/2);

    //Main loop
    SDLKey key = SDLK_UNKNOWN;
    bool keypress = false;
    SDL_Event event;
    int time_now, time_nextMS, time_nextS, last_keypress = 0;
    time_now = time_nextMS = time_nextS = SDL_GetTicks();


    //init AntTweakBar
    InitTweakBar();


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
            fps = cycle;
            cycle = 0;
        }

        //call drawing functions
        Redraw();
        if(draw_ui)
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
                    MouseMotion(event);
                    MouseClick(event);
                }
            }
            else    //update values bound with tweak bar
            {
                //if(event.type != SDL_MOUSEMOTION)
                    UpdateTweakBar();
            }
        }

        //call keyboard handle when key pressed
        if(keypress && (time_now - last_keypress > 150 || last_keypress == time_now) ) 
            KeyInput(key);
    }

    //deinitialize SDL and scene
    delete s;
    TwTerminate();
    SDL_Quit();
    return 0;
}
