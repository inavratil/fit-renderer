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

glm::vec3( 128.167,237.669,501.346 ),
glm::vec3( -0.098088,-0.342112,-0.934532 ),
glm::vec3( -0.0357116,0.939667,-0.340242 ),
glm::vec3( 125.231,37.9995,70.1004 ),
glm::vec3( 0.0174247,-0.139273,-0.990107 ),
glm::vec3( 0.00245074,0.990262,-0.139251 ),
glm::vec3( 140.919,246.406,-253.487 ),
glm::vec3( -0.274062,-0.68211,0.677962 ),
glm::vec3( -0.255642,0.731264,0.632393 ),
glm::vec3( -28.608,20.4059,-237.97 ),
glm::vec3( 0.0653535,-0.342167,0.937374 ),
glm::vec3( 0.0237974,0.939652,0.341337 ),
glm::vec3( -26.6369,6.78971,104.512 ),
glm::vec3( -0.0522607,0.104371,0.993174 ),
glm::vec3( 0.00548404,0.994551,-0.10423 ),
glm::vec3( -175.521,142.533,61.1521 ),
glm::vec3( -0.0521226,-0.000175435,-0.998651 ),
glm::vec3( -8.55403e-006,1.00001,-0.000173304 ),
glm::vec3( -179.924,173.168,-230.307 ),
glm::vec3( 0.356051,-0.309203,0.881842 ),
glm::vec3( 0.115763,0.951012,0.286714 ),
glm::vec3( 110.833,41.2274,-27.4654 ),
glm::vec3( 0.68394,-0.309206,0.660788 ),
glm::vec3( 0.222372,0.951012,0.214845 ),
glm::vec3( 128.167,237.669,501.346 ),
glm::vec3( -0.098088,-0.342112,-0.934532 ),
glm::vec3( -0.0357116,0.939667,-0.340242 )

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
