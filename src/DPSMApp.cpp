#include "DPSMApp.h"

#include "shadows/DPShadowMap.h"

//-----------------------------------------------------------------------------

DPSMApp::DPSMApp(void)
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
}

//-----------------------------------------------------------------------------

void DPSMApp::CreateContent()
{
	//-- setup light
	TLight* light = m_scene->AddLight( 0, dgrey, white, white, glm::vec3( 0.0, 0.0, 0.0 ), 1e4f );
	light->Move(  glm::vec3( -89, 55,125 ) );
	light->SetType( OMNI );

	//-- load content
	m_scene->LoadScene("data/obj/scenes/car2.3ds");
	
	if( MaterialPtr mat_sklo = m_scene->GetMaterialManager()->GetMaterial("sklo") )
		static_cast<GeometryMaterial*>(mat_sklo)->SetTransparency( 0.7f );
	
	TexturePtr tex_pletivo = m_scene->GetTextureCache()->CreateFromImage( "data/tex/alpha/fence.tga" );
	tex_pletivo->SetType( ALPHA );
	if( MaterialPtr mat_pletivo = m_scene->GetMaterialManager()->GetMaterial("pletivoplate") )
		mat_pletivo->AddTexture( tex_pletivo, "tex_pletivo" );

	//-- setup camera
	m_scene->SetFreelookCamera( glm::vec3(-13.1099,7.05098,-207.398 ), glm::vec3(0, 1, 0), glm::vec3(-13.1099,7.05098,-207.398)+glm::vec3(0, 0, -1000) );
	m_scene->AdjustFreelookCamera(-4,-169);

	//-- Dual-Paraboloid shadow technique
	DPShadowMap* shadow_technique = new DPShadowMap( m_scene );
	shadow_technique->SetLight( light );
	shadow_technique->SetShadowParams( 1024, 0.3f );
	m_scene->AddRenderListener( shadow_technique );
}

//-----------------------------------------------------------------------------

void DPSMApp::UpdateScene()
{
	Application::UpdateScene();
}

//-----------------------------------------------------------------------------
