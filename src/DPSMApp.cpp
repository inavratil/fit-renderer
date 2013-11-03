#include "DPSMApp.h"

#include "sdk/ScreenSpaceMaterial.h"

//-----------------------------------------------------------------------------

DPSMApp::DPSMApp(void) :
	m_param_is_drawSM_enabled( false ),
	m_param_is_draw_error_enabled( false )
{
}

//-----------------------------------------------------------------------------

DPSMApp::~DPSMApp(void)
{
}

//-----------------------------------------------------------------------------
void DPSMApp::InitGUI()
{
	Application::InitGUI();

	//show shadow maps
	TwAddVarRW( m_gui, "drawSM", TW_TYPE_BOOLCPP, &m_param_is_drawSM_enabled, 
               " label='Show shadow maps' group='DPSM' key=t ");
	//draw aliasing error
    TwAddVarRW( m_gui, "draw_error", TW_TYPE_BOOLCPP, &m_param_is_draw_error_enabled, 
               " label='Show alias error' group='DPSM' key=f ");
}

//-----------------------------------------------------------------------------

void DPSMApp::CreateContent()
{
	//-- setup light
	TLight* light = m_scene->AddLight( 0, silver, white, white, glm::vec3( 0.0, 0.0, 0.0 ), 5000.0f );
	light->SetType( OMNI );
	m_scene->MoveLight( 0,  glm::vec3( -89, 155, 125 ) );
	

	//-- load content
	m_scene->LoadScene("data/obj/scenes/car2.3ds");
	
	if( MaterialPtr mat_sklo = m_scene->GetMaterialManager()->GetMaterial("sklo") )
		static_cast<GeometryMaterial*>(mat_sklo)->SetTransparency( 0.7f );
	
	TexturePtr tex_pletivo = m_scene->GetTextureCache()->CreateFromImage( "data/tex/alpha/fence.tga" );
	tex_pletivo->SetType( ALPHA );
	if( MaterialPtr mat_pletivo = m_scene->GetMaterialManager()->GetMaterial("pletivoplate") )
		mat_pletivo->AddTexture( tex_pletivo, "alpha_tex" );	//TODO: jmeno jako konstantu ??

	//-- setup camera
	m_scene->SetFreelookCamera( glm::vec3(-13.1099,7.05098,-207.398 ), glm::vec3(0, 1, 0), glm::vec3(-13.1099,7.05098,-207.398)+glm::vec3(0, 0, -1000) );
	m_scene->AdjustFreelookCamera(-4,-169);

	//-- Dual-Paraboloid shadow technique
	m_shadow_technique = new DPShadowMap( m_scene );
	m_shadow_technique->SetLight( light );
	m_shadow_technique->SetShadowParams( 1024, 0.3f );
	m_scene->AddRenderListener( m_shadow_technique );

	m_scene->GetMaterialManager()->AddMaterial( new ScreenSpaceMaterial( "show_depth", "data/shaders/showDepth.vert", "data/shaders/showDepth.frag" ) );
}

//-----------------------------------------------------------------------------

void DPSMApp::UpdateScene()
{
	Application::UpdateScene();

	m_shadow_technique	->	SetDrawShadowMap	( m_param_is_drawSM_enabled );
	m_shadow_technique	->	SetDrawAliasError	( m_param_is_draw_error_enabled );
}

//-----------------------------------------------------------------------------
