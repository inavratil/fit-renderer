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


		s->SetFreelookCamera(glm::vec3(10, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
		s->setCameraMovementSpeed(10.0f);

		s->LoadScene("data/obj/scenes/sphere.3ds");
		//s->AddLight(0, dgrey, silver, grey, glm::vec3(0.0,50.0,0.0), 1000.0f);
		s->AddLight(0, dgrey, silver, grey, glm::vec3(0.0,0.0,0.0), 1000.0f);
		
		//s->MoveLight(0, glm::vec3(-120, 350, 0));
		s->MoveLight(0, glm::vec3(0, 200, 0));

#ifdef USE_DP        
		//dual-paraboloid shadow parameters        
		s->SetShadow(0, SHADOW_RES, OMNI, 0.3f, true);
		s->DPSetPCF(use_pcf);
		s->DPShadowMethod( dpshadow_method );         
		s->DPSetTessellation(dpshadow_tess);
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
        s->ObjCastShadow("sky",false);
        s->MatReceiveShadow("mat_sky",false);

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

		//add shadow shader when shadows are enabled (will be sending depth values only)
		s->AddMaterial("_mat_default_shadow");
		s->CustomShader("_mat_default_shadow", "data/shaders/shadow.vert", "data/shaders/shadow.frag");

		//and also for omnidirectional lights with dual-paraboloid
		s->AddMaterial("_mat_default_shadow_omni");
		s->CustomShader("_mat_default_shadow_omni", "data/shaders/shadow_omni.vert", "data/shaders/shadow_omni.frag");

		//Create material for wireframe bounding volume visualization
		s->AddMaterial("__bv_mat");
		s->CustomShader("__bv_mat", "data/shaders/bounding_volumes.vert", "data/shaders/bounding_volumes.frag");

		//optionally, add tessellation for paraboloid projection
		if(s->DPGetTessellation())
		{        
			TShader vert("data/shaders/shadow_omni_tess.vert", "");
			TShader tcon("data/shaders/shadow_omni_tess.tc", "");
			TShader teval("data/shaders/shadow_omni_tess.te", "");
			TShader frag("data/shaders/shadow_omni_tess.frag", "");

			s->AddMaterial("_mat_default_shadow_omni_tess", white, white, white, 64.0, 0.0, 0.0, NONE);
			s->CustomShader("_mat_default_shadow_omni_tess", &vert, &tcon, &teval, NULL, &frag);
		}
    }
    catch(int)
    {
        cerr<<"Initialization failed.";
        return false;
    }
    //s->SetCamType(cam_type);
    return s->PostInit();
}

string name;
//*****************************************************************************
//Main drawing function
bool drawBVs = false;
void Redraw()
{
	s->updateCamera();
	s->updateLightPosition();

    s->Redraw();

	if(drawBVs)
		s->drawBoundingVolumes();

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
		/*
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
		*/

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
					switch(event.key.keysym.sym)
					{
						case SDLK_w:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_FORWARD_DOWN);
							break;
						case SDLK_s:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_BACKWARD_DOWN);
							break;
						case SDLK_a:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_LEFT_DOWN);
							break;
						case SDLK_d:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_RIGHT_DOWN);
							break;

						case SDLK_i:
							s->handleLightInputMessage(TScene::LIGHT_FORWARD_DOWN);
							break;
						case SDLK_k:
							s->handleLightInputMessage(TScene::LIGHT_BACKWARD_DOWN);
							break;
						case SDLK_j:
							s->handleLightInputMessage(TScene::LIGHT_LEFT_DOWN);
							break;
						case SDLK_l:
							s->handleLightInputMessage(TScene::LIGHT_RIGHT_DOWN);
							break;
						case SDLK_u:
							s->handleLightInputMessage(TScene::LIGHT_UPWARD_DOWN);
							break;
						case SDLK_o:
							s->handleLightInputMessage(TScene::LIGHT_DOWNWARD_DOWN);
							break;
						default:
							break;
					}

                }
                else if(event.type == SDL_KEYUP)
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							delete s;
							SDL_Quit();
							exit(0);
							break;
						case SDLK_w:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_FORWARD_UP);
							break;
						case SDLK_s:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_BACKWARD_UP);
							break;
						case SDLK_a:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_LEFT_UP);
							break;
						case SDLK_d:
							s->handleCameraInputMessage(TFreelookCamera::CAMERA_RIGHT_UP);
							break;
						
						case SDLK_i: 
							s->handleLightInputMessage(TScene::LIGHT_FORWARD_UP);
							break;
						case SDLK_k: 
							s->handleLightInputMessage(TScene::LIGHT_BACKWARD_UP);
							break;
						case SDLK_j: 
							s->handleLightInputMessage(TScene::LIGHT_LEFT_UP);
							break;
						case SDLK_l: 
							s->handleLightInputMessage(TScene::LIGHT_RIGHT_UP);
							break;
						case SDLK_u: 
							s->handleLightInputMessage(TScene::LIGHT_UPWARD_UP);
							break;
						case SDLK_o: 
							s->handleLightInputMessage(TScene::LIGHT_DOWNWARD_UP);
							break;
						case SDLK_b:
							drawBVs = !drawBVs;
							break;
						default:
							break;
					}

				}
                //mouse events
                else if(event.type == SDL_MOUSEMOTION)
                {
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						s->adjustFreelookCamera(0.5f*event.motion.yrel, 0.5f*event.motion.xrel);
					}
                }
            }
            else    //update values bound with tweak bar
            {
                //if(event.type != SDL_MOUSEMOTION)
                    UpdateTweakBar();
            }
        }
    }

    //deinitialize SDL and scene
    delete s;
    TwTerminate();
    SDL_Quit();
    return 0;
}
