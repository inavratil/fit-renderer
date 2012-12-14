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

		s->SetShadowTechnique( new PolynomialWarpedShadow() );

        //-- Loading experiments settings
        if( ex >= 0 )
        {
            if( ex < EXP_COUNT )
            {
                exper = experiments[ex];
                isExperiment = true;
                scene = exper.scene;
                cout << "Experiment loaded." << endl;
            }
            else
            {
                cerr << "WARNING: Experiment with id " << ex << " out of range." << endl;
                throw ERR;
            }
        }


        // axes
        s->AddMaterial("mat_red",red,red,red,0.0,0.0,0.0,NONE);
        s->AddMaterial("mat_green",green,green,green,0.0,0.0,0.0,NONE);
        s->AddMaterial("mat_blue",blue,blue,blue,0.0,0.0,0.0,NONE);

        /*
        // X
        s->AddObject("axisX",CUBE,1.0,1.0);
        s->MoveObj("axisX", 600.0, 0.0, 0.0 );
        s->ResizeObj("axisX", 100.0, 1.0, 1.0 );
        s->SetMaterial("axisX","mat_red");
        s->CastShadow("axisX",false);
        // Y
        s->AddObject("axisY",CUBE,1.0,1.0);
        s->MoveObj("axisY", 500.0, 100.0, 0.0 );
        s->ResizeObj("axisY", 1.0, 100.0, 1.0 );
        s->SetMaterial("axisY","mat_green");
        s->CastShadow("axisY",false);
        // Z
        s->AddObject("axisZ",CUBE,1.0,1.0);
        s->MoveObj("axisZ", 500.0, 0.0, 100.0 );
        s->ResizeObj("axisZ", 1.0, 1.0, 100.0 );
        s->SetMaterial("axisZ","mat_blue");
        s->CastShadow("axisZ",false);
        
        s->ReceiveShadow("mat_red",false);
        s->ReceiveShadow("mat_green",false);
        s->ReceiveShadow("mat_blue",false);
        */

        //skybox
        s->AddMaterial("mat_sky",white,white,white,0.0,0.0,0.0,NONE);
        s->AddTexture("mat_sky",cubemap);
        s->AddObject("sky",CUBE,5000.0,5000.0);
        s->SetMaterial("sky","mat_sky");

        //s->AddObject("camera",CUBE, 10, 10);//"data/obj/camera.3ds");
        //s->AddMaterial("mat_camera", black, green);
        //s->SetMaterial("camera", "mat_camera");

        //scene 1 - car
        if(scene == 1)
        {
            rot = glm::vec3(7, -40, 0);
            pos = glm::vec3(-126,-85, -150);
            
            s->AddLight(0, dgrey, white, white, glm::vec3(-80, 180, 125));
            
            s->LoadScene("data/obj/scenes/car.3ds");
            //glass
            s->SetTransparency("sklo",0.7f);

            s->AddTexture("pletivo", "data/tex/alpha/fence.tga",ALPHA);
            s->AddTexture("body",cubemap, CUBEMAP_ENV, ADD, 0.4f);
            s->AddTexture("sklo",cubemap, CUBEMAP_ENV, ADD, 0.4f);
            s->AddTexture("zem",cubemap, CUBEMAP_ENV, ADD, 0.2f);
            s->AddTexture("chrom",cubemap, CUBEMAP_ENV, ADD, 0.2f);
            s->AddTexture("zrcatko",cubemap, CUBEMAP_ENV, ADD, 0.2f);
            s->AddTexture("cihle","data/tex/depth/bricks.tga",PARALLAX,MODULATE,0.0001f);
        }
        //scene 2 - cathedral
        else if(scene == 2)
        {
            pos = glm::vec3(-540, -43, 0);
            rot = glm::vec3(-20, 270, 0.0f);

			s->AddLight(0, dgrey, white, white, glm::vec3(0.0,50.0,0.0), 1000.0f);

            //s->LoadScene("data/obj/scenes/poor_sibenik.3ds");
            //s->MoveLight(0, glm::vec3(-120, 350, 0));

			s->LoadScene("data/obj/scenes/testing.3ds");
			s->MoveLight(0, glm::vec3(123,98,-43) );
        }
        //scene 3 - bad scene
        else if(scene == 3)
        {
            rot = glm::vec3(17.0, 0.0f, 0.0);
            pos = glm::vec3(0.0f, -30.0, -210.0f);

            //s->LoadScene("../data/obj/scenes/zla_scena.3ds");
            s->AddLight(0, dgrey, white, white, glm::vec3(0.0,75.0,-150.0));

            //s->AddObject("ground",PLANE,1000.0,1000.0);
	        s->AddObject("ground","data/obj/plane.3ds");            
            s->AddObject("cube",CUBE,10.0,10.0);
            s->MoveObj("cube", 100.0, 10.0, 100.0 );
            s->SetMaterial("cube","mat_green");

            //add materials
            s->AddMaterial("mat_ground",silver,silver);
            s->AddMaterial("mat_cone",green,white);            
            //set materials
            s->SetMaterial("ground","mat_ground");

            int CONE_COUNT = 10;
            string cone;

            for(int i=0; i<CONE_COUNT; ++i)
            {
                cone = "cone" + num2str(i);
                s->AddObject(cone.c_str(), "data/obj/cone.3ds");

                float movx = (i%5)*50.0f - 10.0f*CONE_COUNT;
                float movz = (i/5)*200.0f - 100.0f;

                s->MoveObj(cone.c_str(), movx, 0.0, movz );                
                s->SetMaterial(cone.c_str(),"mat_cone");
            }           
        }
        //scene 4 - outdoor
        else if(scene == 4)
        {
            rot = glm::vec3(25.0f, 45.0f, 0.0f);
            pos = glm::vec3(130.0f, -70.0f, -100.0f);
            s->AddLight(0, dgrey, white, white, glm::vec3(30.0, 40.0, 30.0));

            s->AddObject("ground",PLANE,1000.0,1000.0);

            //add materials
            s->AddMaterial("mat_bark",lgrey,white);
            s->AddMaterial("mat_leaf",lgrey,white);
            s->AddMaterial("mat_ground",silver,silver);

            //add textures
            s->AddTexture("mat_bark","data/tex/bark4.tga");
            s->AddTexture("mat_bark","data/tex/normal/bark4_nm.tga",BUMP,MODULATE, 1.0);
            s->AddTexture("mat_leaf","data/tex/alpha/leafs4.tga",ALPHA);
            s->AddTexture("mat_ground","data/tex/grass.tga",BASE, MODULATE, 1.0, 50.0, 100.0);

            //set materials
            s->SetMaterial("ground","mat_ground");

            //trees
            float rnd;
            int OBJECT_COUNT = 7;
            string bark, leaf;
            for(int i=0; i<OBJECT_COUNT; i++)
            {
                for(int j=0; j<OBJECT_COUNT; j++)
                {
                    rnd = rand()%100 / 25.0f - 2.0f;
                    bark = "tree" + num2str(i*OBJECT_COUNT + j) + "_bark";
                    leaf = "tree" + num2str(i*OBJECT_COUNT + j) + "_leaf";
                    //two tree types
                    if( (i*OBJECT_COUNT + j)%2 == 1)
                    {
                        s->AddObject(bark.c_str(),"data/obj/trees/tree1_bark.3ds");
                        s->AddObject(leaf.c_str(),"data/obj/trees/tree1_leaf.3ds");
                    }
                    else
                    {
                        s->AddObject(bark.c_str(),"data/obj/trees/tree2_bark.3ds");
                        s->AddObject(leaf.c_str(),"data/obj/trees/tree2_leaf.3ds");
                    }
                    float movx = j*50.0f + rnd - 25.0f*OBJECT_COUNT;
                    float movz = i*50.0f - rnd - 25.0f* OBJECT_COUNT;
                    float roty = 10.0f * rnd;
                    s->MoveObj(bark.c_str(), movx, 0.0, movz);
                    s->MoveObj(leaf.c_str(), movx, 0.0, movz);
                    s->RotateObj(bark.c_str(), roty, A_Z);
                    s->RotateObj(leaf.c_str(), roty, A_Z);
                    s->SetMaterial(bark.c_str(), "mat_bark");
                    s->SetMaterial(leaf.c_str(), "mat_leaf");
                }
            }
           
            //big tree
            s->AddObject("bigtree_bark","data/obj/trees/tree2_bark.3ds");
            s->ResizeObj("bigtree_bark", 10.0,10.0,10.0);
            s->AddObject("bigtree_leaves","data/obj/trees/tree2_leaf.3ds");
            s->ResizeObj("bigtree_leaves", 10.0,10.0,10.0);
            s->SetMaterial("bigtree_bark","mat_bark");
            s->SetMaterial("bigtree_leaves","mat_leaf");
        }      
        else if(scene == 5)
        {
            //pos = glm::vec3(0, -5, 12.071);
            //rot = glm::vec3(0, 180, 0);
            pos = glm::vec3(17.1, -5, 5);
            rot = glm::vec3(0, 90, 0);

            //s->LoadScene("data/obj/scenes/zla_scena.3ds");
            s->AddObject("cube","data/obj/dbg_plane.3ds");            
            s->SetMaterial("cube","mat_green");

            s->AddLight(0, dgrey, white, white, glm::vec3(0.0,0.0,0.0), 1000.0f);
            s->MoveLight(0, glm::vec3(0, 5, -1));
            //s->MoveLight(0, glm::vec3(0, 5, -12.071));

            parab_rot.y = 90.0;

            //rot = glm::vec3(7, -40, 0);
            //pos = glm::vec3(-126,-85, -150);
            //s->LoadScene("../data/obj/scenes/skoda_low_poly.3ds");
            //s->AddLight(0, dgrey, white, white, glm::vec3(-80, 180, 125));
        }

        if( isExperiment )
        {
            s->MoveLight( 0, exper.light_pos );
            pos = exper.cam_pos;
            rot = exper.cam_rot;
            cut_angle = glm::vec2( exper.cut_params.x, exper.cut_params.y );
            parab_rot.x = exper.cut_params.z;
            parab_rot.y = exper.cut_params.w;

            //s->MoveObj("camera", pos.x, pos.y, pos.z);
            //s->RotateObj("camera", rot.x, A_X);
            //s->RotateObj("camera", rot.y, A_Y);
        }

#ifdef USE_DP        
        //dual-paraboloid shadow parameters        
        s->SetShadow(0, SHADOW_RES, OMNI, 0.3f, true);
        s->DPSetPCF(use_pcf);
        s->DPSetCutAngle( cut_angle );
        s->DPShadowMethod( dpshadow_method );         
        s->DPSetTessellation(dpshadow_tess);
        s->DPDrawSM(drawSM);
        s->DPDrawAliasError(draw_error);
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
        //s->AddMaterial("show_aliasError");
        //s->CustomShader("show_aliasError", "data/shaders/showDepth.vert", "data/shaders/show_aliasError.frag");
        
		//draw quad
        s->AddMaterial("mat_quad",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
        s->CustomShader("mat_quad","data/shaders/quad.vert", "data/shaders/quad.frag");
		s->AddMaterial("mat_quad_lod",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
        s->CustomShader("mat_quad_lod","data/shaders/quad.vert", "data/shaders/quad_lod.frag");
		s->AddMaterial("mat_quad_array",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
        s->CustomShader("mat_quad_array","data/shaders/quad.vert", "data/shaders/quad_array.frag");
        

		//shader showing shadow map alias error
		s->AddMaterial("mat_aliasError");
		s->AddTexture("mat_aliasError", "data/tex/error_color.tga");
		s->CustomShader("mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag");
	  	
		//add shadow shader when shadows are enabled (will be sending depth values only)
		s->AddMaterial("_mat_default_shadow");
		s->CustomShader("_mat_default_shadow", "data/shaders/shadow.vert", "data/shaders/shadow.frag");

		string defines;
		if(dpshadow_method == CUT)
			defines = "#define PARABOLA_CUT\n";

		//and also for omnidirectional lights with dual-paraboloid
		s->AddMaterial("_mat_default_shadow_omni");
		s->CustomShader("_mat_default_shadow_omni", "data/shaders/shadow_omni.vert", "data/shaders/shadow_omni.frag");

		//optionally, add tessellation for paraboloid projection
		if(s->DPGetTessellation())
		{        
			TShader vert("data/shaders/shadow_omni_tess.vert", "");
			TShader tcon("data/shaders/shadow_omni_tess.tc", "");
			TShader teval("data/shaders/shadow_omni_tess.te", "");
			TShader frag("data/shaders/shadow_omni_tess.frag", "");

			s->AddMaterial("_mat_default_shadow_omni_tess", white, white, white, 64.0, 0.0, 0.0, NONE);
			s->CustomShader("_mat_default_shadow_omni_tess", &vert, &tcon, &teval, NULL, &frag);

			TShader warp_vert("data/shaders/shadow_warp_tess.vert", "");
			TShader warp_tcon("data/shaders/shadow_warp_tess.tc", "");
			TShader warp_teval("data/shaders/shadow_warp_tess.te", "");
			TShader warp_frag("data/shaders/shadow_warp_tess.frag", "");

			s->AddMaterial("_mat_shadow_warp_tess", white, white, white, 64.0, 0.0, 0.0, NONE);
			s->CustomShader("_mat_shadow_warp_tess", &warp_vert, &warp_tcon, &warp_teval, NULL, &warp_frag);
		}
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
    dp_farPoint = s->DPGetFarPoint();

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
    case SDLK_7:
        cut_angle.y -= 20.0;
        s->DPSetCutAngle(cut_angle);
        break;
    case SDLK_8:
        cut_angle.y += 20.0;
        s->DPSetCutAngle(cut_angle);
        break;
    case SDLK_9:
        cut_angle.x -= 15.0;
        s->DPSetCutAngle(cut_angle);
        break;
    case SDLK_0:
        cut_angle.x += 15.0;
        s->DPSetCutAngle(cut_angle);
        break;

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

	case SDLK_t:
		drawSM = !drawSM;
		s->DPDrawSM(drawSM);
        break;
    case SDLK_ESCAPE:        //ESCAPE - quit
        delete s;
        SDL_Quit();
        exit(0);
        break;

    default:
        break;
    }
	//cout<<"LIGHT: "<<lpos1.x<<","<<lpos1.y<<","<<lpos1.z<<endl;
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
        ///////////////////////////////////////////////
        //scene selection
        else if(param == "-scene")
        {
            if(i+1 < argc)
            {
                scene = atoi(argv[i+1]);
                i++;
            }
            else
                WrongParams();
        }
        ///////////////////////////////////////////////
        //experiment selection
        else if(param == "-exp")
        {
            if(i+1 < argc)
            {
                ex = atoi(argv[i+1]);
                i++;
            }
            else
                WrongParams();
        }
        ///////////////////////////////////////////
        //parabola cut
        else if(param == "-cut")
        {
            parabola_cut = true;
            dpshadow_method = CUT;
        }
        else if(param == "-dpsm")
            dpshadow_method = DPSM;
        else if(param == "-ipsm")
            dpshadow_method = IPSM;
		else if(param == "-warp")
            dpshadow_method = WARP_DPSM;
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
        else if(param == "-noui")
            draw_ui = false;

        ///////////////////////////////////////////
        //error
        else
            WrongParams();
    }

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
