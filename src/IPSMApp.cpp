#include "IPSMApp.h"

//-----------------------------------------------------------------------------

IPSMApp::IPSMApp(void) :
	drawSM( true ),
	cut_angle( 0 )
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

void IPSMApp::InitGUI()
{
	Application::InitGUI();

	if( !m_gui ) return;

#if 0
    TwAddVarRW( m_gui, "wire", TW_TYPE_BOOL32, &wire, 
               " label='Wireframe' group='Scene' key=x");
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
	//IPSM textures
	TwEnumVal ipsm_tex[5] = { {OUTPUT, "Ouput"}, {PING, "Ping"}, {PONG, "Pong"}, {MASK, "Mask"}, {CAM_ERR, "Camera error"} };
	TwType ipsm_texs = TwDefineEnum("ipsm_texs", ipsm_tex, 5);
    TwAddVarRW( m_gui, "ipsm_tex", ipsm_texs, &ipsm_texPrev, "label='Texture' group='Shadows'");
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
    //draw aliasing error
    TwAddVarRW( m_gui, "draw_error", TW_TYPE_BOOLCPP, &draw_error, 
               " label='Show alias error' group='Shadows' key=f ");
    //show shadow maps
    TwAddVarRW( m_gui, "drawSM", TW_TYPE_BOOLCPP, &drawSM, 
               " label='Show shadow maps' group='Shadows' ");

	//do warping
    TwAddVarRW( m_gui, "do_warp", TW_TYPE_BOOLCPP, &do_warp, 
               " label='Enable warping' group='Shadows' key=v ");

    //bar settings
    TwDefine("TweakBar refresh=0.1 size='256 512' ");
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

void IPSMApp::KeyPressed(SDLKey key)
{
	Application::KeyPressed( key );

    switch(key)
    {     
    case SDLK_7:
        cut_angle.y -= 20.0;
        m_scene->DPSetCutAngle(cut_angle);
        break;
    case SDLK_8:
        cut_angle.y += 20.0;
        m_scene->DPSetCutAngle(cut_angle);
        break;
    case SDLK_9:
        cut_angle.x -= 15.0;
        m_scene->DPSetCutAngle(cut_angle);
        break;
    case SDLK_0:
        cut_angle.x += 15.0;
        m_scene->DPSetCutAngle(cut_angle);
        break;
	case SDLK_t:
		drawSM = !drawSM;
		m_scene->DPDrawSM(drawSM);
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------