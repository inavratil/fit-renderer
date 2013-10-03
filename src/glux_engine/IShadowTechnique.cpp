#include "IShadowTechnique.h"


IShadowTechnique::IShadowTechnique( ScenePtr _scene ) :
	RenderListener( _scene )
{
	_Init();
}

IShadowTechnique::~IShadowTechnique(void)
{
	_Destroy();
}

void IShadowTechnique::_Init()
{
	m_iTexID = 0;
	m_pScreenGrid = new ScreenGrid( m_cDefaultRes );
}

void IShadowTechnique::_Destroy()
{
	delete m_pScreenGrid;
	delete m_pShaderFeature;
}

void IShadowTechnique::SetControlPointsCount( float _res )
{
	m_pScreenGrid->SetControlPointsCount( _res ); 
}

float IShadowTechnique::GetControlPointsCount()
{
	return m_pScreenGrid->GetControlPointsCount();
}

const char* IShadowTechnique::GetName()
{
	return m_sName;
}

const char* IShadowTechnique::GetDefines()
{
	return m_sDefines;
}

void IShadowTechnique::Enable()
{
	m_bEnabled = true;
}

void IShadowTechnique::Disable()
{
	m_bEnabled = false;
}

bool IShadowTechnique::IsEnabled()
{
	return m_bEnabled;
}

GLuint IShadowTechnique::GetTexId()
{ 
	return m_iTexID; 
}

void IShadowTechnique::SetTexId( GLuint _texid)
{ 
	m_iTexID = _texid; 
}

ScreenGrid* IShadowTechnique::GetGrid()
{
	return m_pScreenGrid;
}

void IShadowTechnique::SetShaderFeature( ShaderFeature* _pFeature )
{
	m_pShaderFeature = _pFeature;
}

ShaderFeature* IShadowTechnique::GetShaderFeature()
{
	return m_pShaderFeature;
}

void IShadowTechnique::UpdateGridRange( glm::vec4 _range )
{
	m_pScreenGrid->UpdateRange(_range);
}

glm::vec4 IShadowTechnique::GetGridRange()
{
	return m_pScreenGrid->GetRange();
}

void IShadowTechnique::GenerateGrid()
{
	m_pScreenGrid->GenerateGrid(this);
}

void IShadowTechnique::DrawGrid()
{
	m_pScreenGrid->Draw();
}

