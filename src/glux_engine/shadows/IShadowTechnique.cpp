#include "IShadowTechnique.h"

#include "sdk/ScreenSpaceMaterial.h"
//-----------------------------------------------------------------------------

IShadowTechnique::IShadowTechnique( TScene* _scene ) :
	RenderListener( _scene )
{
	MaterialManagerPtr material_manager = _scene->GetMaterialManager();
	//add shadow shader when shadows are enabled (will be sending depth values only)
	material_manager->AddMaterial( new ScreenSpaceMaterial( "_mat_default_shadow", "data/shaders/shadow.vert", "data/shaders/shadow.frag" ) );
	//and also for omnidirectional lights with dual-paraboloid
	material_manager->AddMaterial( new ScreenSpaceMaterial( "_mat_default_shadow_omni", "data/shaders/default_shadow_omni.vert", "data/shaders/default_shadow_omni.frag" ) );
}

//-----------------------------------------------------------------------------

IShadowTechnique::~IShadowTechnique(void)
{
}

//-----------------------------------------------------------------------------

TLight* IShadowTechnique::GetLight()
{
	return m_pLight;
}

//-----------------------------------------------------------------------------

void IShadowTechnique::SetLight( TLight* _light )
{
	m_pLight = _light;
}

//-----------------------------------------------------------------------------

void IShadowTechnique::Enable()
{
	m_bEnabled = true;
}

//-----------------------------------------------------------------------------

void IShadowTechnique::Disable()
{
	m_bEnabled = false;
}

//-----------------------------------------------------------------------------

bool IShadowTechnique::IsEnabled()
{
	return m_bEnabled;
}

//-----------------------------------------------------------------------------
