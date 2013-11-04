#include "DPSMApp.h"

#include "sdk/ScreenSpaceMaterial.h"


//-----------------------------------------------------------------------------

void TW_CALL SaveCamera(void *clientData)
{ 
	static_cast<TCamera*>(clientData)->Save();
}


//-----------------------------------------------------------------------------

DPSMApp::DPSMApp(void) :
	m_param_is_drawSM_enabled( true ),
	m_param_is_draw_error_enabled( false ),
	m_param_is_extension_enabled( true )
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

	//-- enable method extension
	TwAddVarRW( m_gui, "method_enabled", TW_TYPE_BOOLCPP, &m_param_is_extension_enabled, 
               " label='Extension enabled' group='DPSM' key=v ");
	//-- show shadow maps
	TwAddVarRW( m_gui, "drawSM", TW_TYPE_BOOLCPP, &m_param_is_drawSM_enabled, 
               " label='Show shadow maps' group='DPSM' key=t ");
	//-- draw aliasing error
    TwAddVarRW( m_gui, "draw_error", TW_TYPE_BOOLCPP, &m_param_is_draw_error_enabled, 
               " label='Show alias error' group='DPSM' key=f ");
	//-- Save camera
	TwAddButton( m_gui, "Save Camera", SaveCamera, m_scene->GetCameraPtr(), " label='SaveCamera' ");

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
	m_shadow_technique = new ImprovedDPShadowMap( m_scene );
	m_shadow_technique->SetLight( light );
	m_shadow_technique->SetShadowParams( 1024, 0.3f );
	m_scene->AddRenderListener( m_shadow_technique );

	m_scene->GetMaterialManager()->AddMaterial( new ScreenSpaceMaterial( "show_depth", "data/shaders/showDepth.vert", "data/shaders/showDepth.frag" ) );

	//-- Set camera animation path
	glm::vec3 cams[] = {
		/*glm::vec3( 129.687,112.919,247.576 ),
		glm::vec3( -0.396288,-0.224967,-0.89014 ),
		glm::vec3( -0.0914968,0.974368,-0.20552 ),
		glm::vec3( -13.1099,7.05098,-207.398 ),
		glm::vec3( -0.190344,0.0697564,0.979236 ),
		glm::vec3( 0.0133102,0.997564,-0.0684748 )*/
glm::vec3( 133.958,215.618,566.784 ),
glm::vec3( -0.0500268,-0.292398,-0.954988 ),
glm::vec3( -0.0152962,0.956298,-0.291998 ),
glm::vec3( 127.43,39.3507,50.3098 ),
glm::vec3( -0.22079,-0.19084,-0.956469 ),
glm::vec3( -0.0429242,0.981622,-0.185949 ),
glm::vec3( 75.3756,193.923,-243.236 ),
glm::vec3( -0.336792,-0.71937,0.60752 ),
glm::vec3( -0.348788,0.69463,0.629158 ),
glm::vec3( -25.032,28.8676,-111.571 ),
glm::vec3( -0.0407154,-0.629359,0.77605 ),
glm::vec3( -0.0329739,0.777118,0.628495 ),
glm::vec3( -137.585,160.161,152.631 ),
glm::vec3( 0.492986,-0.544688,-0.678442 ),
glm::vec3( 0.320189,0.838643,-0.440641 ),
glm::vec3( -189.234,144.074,-3.01823 ),
glm::vec3( 0.291326,-0.0872128,-0.952643 ),
glm::vec3( 0.0255043,0.996193,-0.0833997 ),
glm::vec3( -96.0929,124.256,-224.82 ),
glm::vec3( 0.421826,-0.57363,0.702153 ),
glm::vec3( 0.295405,0.819119,0.491718 ),
glm::vec3( 17.8991,76.9393,-224.055 ),
glm::vec3( 0.336685,-0.342082,0.877286 ),
glm::vec3( 0.122568,0.939675,0.319369 ),
glm::vec3( 85.6945,28.5551,-49.9938 ),
glm::vec3( 0.556997,-0.087222,0.825926 ),
glm::vec3( 0.0487678,0.996193,0.0723135 ),
glm::vec3( 59.1936,31.8597,155.359 ),
glm::vec3( -0.604871,-0.275709,-0.747078 ),
glm::vec3( -0.173491,0.961247,-0.214279 ),
glm::vec3( -34.7007,24.0727,38.9545 ),
glm::vec3( 0.0305973,-0.484879,-0.874052 ),
glm::vec3( 0.0169633,0.874588,-0.484581 ),
glm::vec3( 57.5505,23.7403,-225.089 ),
glm::vec3( -0.422446,0.0348222,0.905725 ),
glm::vec3( 0.0147199,0.9994,-0.0315592 ),
glm::vec3( 133.958,215.618,566.784 ),
glm::vec3( -0.0500268,-0.292398,-0.954988 ),
glm::vec3( -0.0152962,0.956298,-0.291998 )

	};


	std::vector<TCam> keyframes;

	for( int i=0; i< sizeof(cams)/sizeof(glm::vec3); i += 3 )
	{
		TCam cam;
		cam.buildView(cams[i], cams[i+1],cams[i+2]);
		keyframes.push_back( cam );
	}

	m_scene->SetAnimationCamera( keyframes );
}

//-----------------------------------------------------------------------------

void DPSMApp::UpdateScene()
{
	Application::UpdateScene();

	m_shadow_technique	->	SetDrawShadowMap	( m_param_is_drawSM_enabled );
	m_shadow_technique	->	SetDrawAliasError	( m_param_is_draw_error_enabled );

	if( m_param_is_extension_enabled )
		m_shadow_technique->Enable();
	else
		m_shadow_technique->Disable();
}

//-----------------------------------------------------------------------------
