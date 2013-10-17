#include "DPSMApp.h"

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
	TLight* light = m_scene->AddLight( 0, dgrey, white, white, glm::vec3( 0.0, 0.0, 0.0 ), 1e4f );
	light->Move(  glm::vec3( -89, 55,125 ) );

	m_scene->LoadScene("data/obj/scenes/car2.3ds");

	m_scene->SetFreelookCamera( glm::vec3(-13.1099,7.05098,-207.398 ), glm::vec3(0, 1, 0), glm::vec3(-13.1099,7.05098,-207.398)+glm::vec3(0, 0, -1000) );
	m_scene->AdjustFreelookCamera(-4,-169);

	m_scene->UpdateCamera();
}

//-----------------------------------------------------------------------------

void DPSMApp::UpdateScene()
{
	Application::UpdateScene();
}

//-----------------------------------------------------------------------------