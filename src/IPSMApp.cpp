#include "IPSMApp.h"

#define USE_DP

bool g_use_pcf = false;
bool g_use_tess = false;
int g_shadow_res = 1024;
int g_dpshadow_method = WARP_DPSM;
glm::vec3 g_parab_rot = glm::vec3( 0 );

//-----------------------------------------------------------------------------

IPSMApp::IPSMApp(void) :
	m_param_is_drawSM_enabled( true ),
	m_param_cut_angle( 0 ),
	m_param_is_warping_enabled( true ),
	m_param_is_draw_error_enabled( false ),
	m_param_preview_texture_id( OUTPUT )
{
}

//-----------------------------------------------------------------------------

IPSMApp::~IPSMApp(void)
{
}

//-----------------------------------------------------------------------------

void IPSMApp::CreateContent()
{ 	
	const char *cubemap[] = {   "data/tex/cubemaps/posx.tga", "data/tex/cubemaps/negx.tga",
		"data/tex/cubemaps/posy.tga", "data/tex/cubemaps/negy.tga",
		"data/tex/cubemaps/posz.tga", "data/tex/cubemaps/negz.tga" };

	//-------------------------------------------------------------------------

	m_scene->AddLight(0, dgrey, white, white, glm::vec3(0.0,0.0,0.0), 1000.0f);

	/*
	ShadowTechnique* shadow_technique = m_scene->CreateShadowTechnique( new SplineWarpedShadow() );
	shadow_technique->Set*( <whatever> );

	light->SetShadowTechnique( shadow_technique );

	*/

	int scene = SetupExperiments( "experiments.cfg" );
	//skybox
	//s->AddMaterial("mat_sky",white,white,white,0.0,0.0,0.0,NONE);
	//s->AddTexture("mat_sky",cubemap);
	//s->AddObject("sky",CUBE,5000.0,5000.0);
	//s->SetMaterial("sky","mat_sky");
	//cast/receive shadows
	//s->ObjCastShadow("sky",false);
	//s->MatReceiveShadow("mat_sky",false);

	//s->AddObject("camera",CUBE, 10, 10);//"data/obj/camera.3ds");
	//s->AddMaterial("mat_camera", black, green);
	//s->SetMaterial("camera", "mat_camera");

	//scene 1 - car
	if(scene == 1)
	{            
		m_scene->LoadScene("data/obj/scenes/car.3ds");
		//glass
		//FIXME: nastavit v LoadScene: s->SetTransparency("sklo",0.7f);

		//s->AddTexture("pletivo", "data/tex/alpha/fence.tga",ALPHA);
		//s->AddTexture("body",cubemap, CUBEMAP_ENV, ADD, 0.4f);
		//s->AddTexture("sklo",cubemap, CUBEMAP_ENV, ADD, 0.4f);
		//s->AddTexture("zem",cubemap, CUBEMAP_ENV, ADD, 0.2f);
		//s->AddTexture("chrom",cubemap, CUBEMAP_ENV, ADD, 0.2f);
		//s->AddTexture("zrcatko",cubemap, CUBEMAP_ENV, ADD, 0.2f);
		//s->AddTexture("cihle","data/tex/depth/bricks.tga",PARALLAX,MODULATE,0.0001f);
	}
	//scene 2 - cathedral
	else if(scene == 2)
	{
		m_scene->LoadScene("data/obj/scenes/sibenik.3ds");
		//m_scene->LoadScene("data/obj/scenes/testing_tess_all.3ds");
	}
	//scene 3 - bad scene
	else if(scene == 3)
	{
		//s->LoadScene("../data/obj/scenes/zla_scena.3ds");

		//s->AddObject("ground",PLANE,1000.0,1000.0);
		m_scene->AddObject("ground","data/obj/plane.3ds");            
		m_scene->AddObject("cube",CUBE,10.0,10.0);
		m_scene->MoveObj("cube", 100.0, 10.0, 100.0 );
		m_scene->SetMaterial("cube","mat_green");

		//add materials
		m_scene->AddMaterial("mat_ground",silver,silver);
		m_scene->AddMaterial("mat_cone",green,white);            
		//set materials
		m_scene->SetMaterial("ground","mat_ground");

		int CONE_COUNT = 10;
		string cone;

		for(int i=0; i<CONE_COUNT; ++i)
		{
			cone = "cone" + num2str(i);
			m_scene->AddObject(cone.c_str(), "data/obj/cone.3ds");

			float movx = (i%5)*50.0f - 10.0f*CONE_COUNT;
			float movz = (i/5)*200.0f - 100.0f;

			m_scene->MoveObj(cone.c_str(), movx, 0.0, movz );                
			m_scene->SetMaterial(cone.c_str(),"mat_cone");
		}           
	}
	//scene 4 - outdoor
	else if(scene == 4)
	{
		m_scene->AddObject("ground",PLANE,1000.0,1000.0);

		//add materials
		m_scene->AddMaterial("mat_bark",lgrey,white);
		m_scene->AddMaterial("mat_leaf",lgrey,white);
		m_scene->AddMaterial("mat_ground",silver,silver);

		//add textures
		//s->AddTexture("mat_bark","data/tex/bark4.tga");
		//s->AddTexture("mat_bark","data/tex/normal/bark4_nm.tga",BUMP,MODULATE, 1.0);
		//s->AddTexture("mat_leaf","data/tex/alpha/leafs4.tga",ALPHA);
		//s->AddTexture("mat_ground","data/tex/grass.tga",BASE, MODULATE, 1.0, 50.0, 100.0);

		//set materials
		m_scene->SetMaterial("ground","mat_ground");

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
					m_scene->AddObject(bark.c_str(),"data/obj/trees/tree1_bark.3ds");
					m_scene->AddObject(leaf.c_str(),"data/obj/trees/tree1_leaf.3ds");
				}
				else
				{
					m_scene->AddObject(bark.c_str(),"data/obj/trees/tree2_bark.3ds");
					m_scene->AddObject(leaf.c_str(),"data/obj/trees/tree2_leaf.3ds");
				}
				float movx = j*50.0f + rnd - 25.0f*OBJECT_COUNT;
				float movz = i*50.0f - rnd - 25.0f* OBJECT_COUNT;
				float roty = 10.0f * rnd;
				m_scene->MoveObj(bark.c_str(), movx, 0.0, movz);
				m_scene->MoveObj(leaf.c_str(), movx, 0.0, movz);
				m_scene->RotateObj(bark.c_str(), roty, A_Z);
				m_scene->RotateObj(leaf.c_str(), roty, A_Z);
				m_scene->SetMaterial(bark.c_str(), "mat_bark");
				m_scene->SetMaterial(leaf.c_str(), "mat_leaf");
			}
		}

		//big tree
		m_scene->AddObject("bigtree_bark","data/obj/trees/tree2_bark.3ds");
		m_scene->ResizeObj("bigtree_bark", 10.0,10.0,10.0);
		m_scene->AddObject("bigtree_leaves","data/obj/trees/tree2_leaf.3ds");
		m_scene->ResizeObj("bigtree_leaves", 10.0,10.0,10.0);
		m_scene->SetMaterial("bigtree_bark","mat_bark");
		m_scene->SetMaterial("bigtree_leaves","mat_leaf");
	}      


	//---------------------------------------------------------------------

#ifdef USE_DP        
	//dual-paraboloid shadow parameters        
	m_scene->SetShadow(0, g_shadow_res, OMNI, 0.3f, true);
	m_scene->DPSetPCF( g_use_pcf );
	//s->DPSetCutAngle( m_param_cut_angle );
	m_scene->DPShadowMethod( g_dpshadow_method );         
	m_scene->DPSetTessellation( g_use_tess );
	m_scene->RotateParaboloid( g_parab_rot );
#else
	s->SetShadow(0, SHADOW_RES, SPOT, 0.3f);
#endif

	//---------------------------------------------------------------------

	//toggle effects (HDR & SSAO)
	//s->UseHDR();
	//s->UseSSAO();

	//prepare render to texture
	//s->CreateHDRRenderTarget(-1, -1, GL_RGBA16F, GL_FLOAT);

	////General HDR settings
	//s->SetUniform("mat_tonemap","exposure",1.0);
	//s->SetUniform("mat_tonemap","bloomFactor",0.3);
	//s->SetUniform("mat_tonemap","brightMax",1.7);
	//s->SetUniform("mat_bloom_hdr_ssao","THRESHOLD",0.5);
	////SSAO settings
	//s->SetUniform("mat_bloom_hdr_ssao","intensity",0.7);
	//s->SetUniform("mat_bloom_hdr_ssao","radius",0.004);
	//s->SetUniform("mat_bloom_hdr_ssao","bias",1.5);

	//---------------------------------------------------------------------

	m_scene->AddMaterial( new ScreenSpaceMaterial( "show_depth", "data/shaders/showDepth.vert", "data/shaders/showDepth.frag" ) );

	//draw quad
	m_scene->AddMaterial( new ScreenSpaceMaterial( "mat_quad","data/shaders/quad.vert", "data/shaders/quad.frag" ) );
	m_scene->AddMaterial( new ScreenSpaceMaterial( "mat_quad_lod","data/shaders/quad.vert", "data/shaders/quad_lod.frag" ) );
	m_scene->AddMaterial( new ScreenSpaceMaterial( "mat_quad_array","data/shaders/quad.vert", "data/shaders/quad_array.frag" ) );        

	//add shadow shader when shadows are enabled (will be sending depth values only)
	m_scene->AddMaterial( new ScreenSpaceMaterial( "_mat_default_shadow", "data/shaders/shadow.vert", "data/shaders/shadow.frag" ) );
	string defines;
	//if(dpshadow_method == CUT)
	//	defines = "#define PARABOLA_CUT\n";

	//and also for omnidirectional lights with dual-paraboloid
	m_scene->AddMaterial( new ScreenSpaceMaterial( "_mat_default_shadow_omni", "data/shaders/shadow_omni.vert", "data/shaders/shadow_omni.frag" ) );

	//optionally, add tessellation for paraboloid projection
	/*
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
	*/

	//-------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------

int IPSMApp::SetupExperiments( const string& _filename )
{
	int scene = 1;
	if( !m_experiment_name.empty() )
	{
		ConfigDialogPtr dialog = new ConfigDialog();
		dialog->Load( _filename );

		string section = m_experiment_name;
		string option;

		option = dialog->GetSetting( "Scene", section );
		scene = StringUtil::ParseInt( option );

		option = dialog->GetSetting( "Light position", section );
		glm::vec3 light_pos = StringUtil::ParseVector3( option );
		option = dialog->GetSetting( "Camera position", section );
		glm::vec3 cam_pos = StringUtil::ParseVector3( option );
		option = dialog->GetSetting( "Camera orientation", section );
		glm::vec3 cam_rot = StringUtil::ParseVector3( option );

		delete dialog;

		m_scene->MoveLight( 0, light_pos );
		//m_param_cut_angle = glm::vec2( exper.cut_params.x, exper.cut_params.y );
		//parab_rot.x = exper.cut_params.z;
		//parab_rot.y = exper.cut_params.w;
		
		m_scene->SetFreelookCamera( glm::vec3(cam_pos.x, cam_pos.y, cam_pos.z), glm::vec3(0, 1, 0), glm::vec3(cam_pos.x, cam_pos.y, cam_pos.z)+glm::vec3(0, 0, -1000) );
		//m_scene->MoveCameraAbs(cam_pos.x, cam_pos.y, cam_pos.z);
		m_scene->adjustFreelookCamera(cam_rot.x, cam_rot.y);
		//m_scene->RotateCameraAbs(cam_rot.x, A_X);
		//m_scene->RotateCameraAbs(cam_rot.y, A_Y);
		m_scene->updateCamera();
	}

	return scene;
}

//-----------------------------------------------------------------------------

void IPSMApp::UpdateScene()
{
	Application::UpdateScene();

	m_scene	->	DPDrawSM			( m_param_is_drawSM_enabled );
	m_scene	->	SetWarping			( m_param_is_warping_enabled );
	m_scene	->	DPDrawAliasError	( m_param_is_draw_error_enabled );
	m_scene	->	SetTexturePreviewId	( m_param_preview_texture_id );
}

//-----------------------------------------------------------------------------

void IPSMApp::InitGUI()
{
	Application::InitGUI();

	if( !m_gui ) return;

	//do warping
	TwAddVarRW( m_gui, "do_warp", TW_TYPE_BOOLCPP, &m_param_is_warping_enabled, 
               " label='Enable warping' group='IPSM' key=v ");
	//show shadow maps
	TwAddVarRW( m_gui, "drawSM", TW_TYPE_BOOLCPP, &m_param_is_drawSM_enabled, 
               " label='Show shadow maps' group='IPSM' key=t ");
	//draw aliasing error
    TwAddVarRW( m_gui, "draw_error", TW_TYPE_BOOLCPP, &m_param_is_draw_error_enabled, 
               " label='Show alias error' group='IPSM' key=f ");
	//IPSM textures
	TwEnumVal ipsm_tex[5] = { {OUTPUT, "Ouput"}, {PING, "Ping"}, {PONG, "Pong"}, {MASK, "Mask"}, {CAM_ERR, "Camera error"} };
	TwType ipsm_texs = TwDefineEnum("ipsm_texs", ipsm_tex, 5);
    TwAddVarRW( m_gui, "ipsm_tex", ipsm_texs, &m_param_preview_texture_id, "label='Texture' group='IPSM'");
#if 0

    //camera
    TwEnumVal e_cam_type[] = { {FPS, "FPS"}, {ORBIT, "Orbit"}};
    TwType e_cam_types = TwDefineEnum("cam_types", e_cam_type, 2);
    TwAddVarRW( m_gui, "cam_mode", e_cam_types, &cam_type, "label='Type' key=c group='Camera'");
    //camera save/load
    TwAddButton( m_gui, "cam_save", twSaveCamera, NULL, "label='Save' group='Camera' key=F5"); 
    TwAddButton( m_gui, "cam_load", twLoadCamera, NULL, "label='Load' group='Camera' key=F9"); 


    //shadow settings   
    TwAddVarRO( m_gui, "sh_res", TW_TYPE_INT32, &SHADOW_RES, 
               " label='Resolution' group='Shadows' ");
    //enum all shadow types
    TwEnumVal sh_mode[3] = { {DPSM, "DPSM"}, {IPSM, "IPSM"}, {CUT, "CUT"} };
    TwType sh_modes = TwDefineEnum("sh_modes", sh_mode, 3);
    TwAddVarRO( m_gui, "sh_mode", sh_modes, &dpshadow_method, "label='Mode' group='Shadows'");

    //parabola cut
    TwAddVarRO( m_gui, "parabola_cut", TW_TYPE_BOOLCPP, &parabola_cut, 
               " label='Parabola cut' group='Shadows' ");
    //near/far point
    TwAddVarRO( m_gui, "frontFOV", TW_TYPE_FLOAT, &dp_frontFOV, 
               " label='Front FOV' group='Shadows' precision=1");
    TwAddVarRO( m_gui, "far_point", TW_TYPE_FLOAT, &dp_farPoint, 
               " label='Far point' group='Shadows' precision=1");

    //toggle paraboloid rotation
    TwAddSeparator( m_gui, NULL, "group='Shadows'");
    TwAddVarRW( m_gui, "move_parab", TW_TYPE_BOOLCPP, &move_parab, 
               " label='Moving paraboloid' group='Shadows' key=q");
    //reset paraboloid
    TwAddButton(m_gui, "reset_parab", twResetParab, NULL, "label='Reset paraboloid' key=r"); 

#endif

}

//-----------------------------------------------------------------------------

void IPSMApp::MouseMoved(SDL_Event event)
{	
	int status = event.button.button;
/*
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
            m_scene->RotateParaboloid(parab_rot);

        }
    }
	else
*/
		Application::MouseMoved( event );

}

//-----------------------------------------------------------------------------

void IPSMApp::KeyPressed( SDLKey _key )
{
	Application::KeyInput( _key, SDL_KEYDOWN );

    switch(_key)
    {
	//-------------------------------------------
	//FIXME: Presunout do Cut shadow technique
	/*
    case SDLK_7:
        m_param_cut_angle.y -= 20.0;
        m_scene->DPSetCutAngle(m_param_cut_angle);
        break;
    case SDLK_8:
        m_param_cut_angle.y += 20.0;
        m_scene->DPSetCutAngle(m_param_cut_angle);
        break;
    case SDLK_9:
        m_param_cut_angle.x -= 15.0;
        m_scene->DPSetCutAngle(m_param_cut_angle);
        break;
    case SDLK_0:
        m_param_cut_angle.x += 15.0;
        m_scene->DPSetCutAngle(m_param_cut_angle);
        break;
	*/
	//-------------------------------------------
	//case SDLK_t:
	//	m_param_is_drawSM_enabled = !m_param_is_drawSM_enabled;
	//	m_scene->DPDrawSM( m_param_is_drawSM_enabled );
    //    break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------