#include "IShadowTechnique.h"

//-----------------------------------------------------------------------------

IShadowTechnique::IShadowTechnique( TScene* _scene ) :
	RenderListener( _scene )
{
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
