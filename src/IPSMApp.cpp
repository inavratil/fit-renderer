#include "IPSMApp.h"

//-----------------------------------------------------------------------------

IPSMApp::IPSMApp(void) :
	m_param_is_drawSM_enabled( true ),
	m_param_cut_angle( 0 ),
	m_param_is_warping_enabled( true ),
	m_param_is_draw_error_enabled( false ),
	m_param_preview_texture_id( OUTPUT ),
	m_scene_id( 1 ) //FIXME: opravdu 1?
{
}

//-----------------------------------------------------------------------------

IPSMApp::~IPSMApp(void)
{
}

//-----------------------------------------------------------------------------

//void IPSMApp::CreateContent( ScenePtr s )
//{
//
//}

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

		m_scene->MoveCameraAbs(cam_pos.x, cam_pos.y, cam_pos.z);
		m_scene->RotateCameraAbs(cam_rot.x, A_X);
		m_scene->RotateCameraAbs(cam_rot.y, A_Y);
	}

	return scene;
}

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
	Application::KeyPressed( _key );

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