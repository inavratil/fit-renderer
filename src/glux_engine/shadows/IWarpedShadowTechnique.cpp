#include "IWarpedShadowTechnique.h"

//-----------------------------------------------------------------------------

IWarpedShadowTechnique::IWarpedShadowTechnique( TScene* _scene ) :
	RenderListener( _scene )
{
	_Init();
}

//-----------------------------------------------------------------------------

IWarpedShadowTechnique::~IWarpedShadowTechnique(void)
{
	_Destroy();
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::_Init()
{
	m_iTexID = 0;
	m_pScreenGrid = new ScreenGrid( m_cDefaultRes );
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::_Destroy()
{
	delete m_pScreenGrid;
	delete m_pShaderFeature;
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::SetControlPointsCount( float _res )
{
	m_pScreenGrid->SetControlPointsCount( _res ); 
}

//-----------------------------------------------------------------------------

float IWarpedShadowTechnique::GetControlPointsCount()
{
	return m_pScreenGrid->GetControlPointsCount();
}

//-----------------------------------------------------------------------------

const char* IWarpedShadowTechnique::GetName()
{
	return m_sName;
}

//-----------------------------------------------------------------------------

const char* IWarpedShadowTechnique::GetDefines()
{
	return m_sDefines;
}

//-----------------------------------------------------------------------------

GLuint IWarpedShadowTechnique::GetTexId()
{ 
	return m_iTexID; 
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::SetTexId( GLuint _texid)
{ 
	m_iTexID = _texid; 
}

//-----------------------------------------------------------------------------

ScreenGrid* IWarpedShadowTechnique::GetGrid()
{
	return m_pScreenGrid;
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::SetShaderFeature( ShaderFeature* _pFeature )
{
	m_pShaderFeature = _pFeature;
}

//-----------------------------------------------------------------------------

ShaderFeature* IWarpedShadowTechnique::GetShaderFeature()
{
	return m_pShaderFeature;
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::UpdateGridRange( glm::vec4 _range )
{
	m_pScreenGrid->UpdateRange(_range);
}

//-----------------------------------------------------------------------------

glm::vec4 IWarpedShadowTechnique::GetGridRange()
{
	return m_pScreenGrid->GetRange();
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::GenerateGrid()
{
	m_pScreenGrid->GenerateGrid(this);
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::DrawGrid()
{
	m_pScreenGrid->Draw();
}

//-----------------------------------------------------------------------------
