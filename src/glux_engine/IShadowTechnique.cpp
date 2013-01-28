#include "IShadowTechnique.h"


IShadowTechnique::IShadowTechnique()
{
	_Init( 0 );
}

IShadowTechnique::IShadowTechnique( GLuint _texid ) 
{
	_Init( _texid );
}

IShadowTechnique::~IShadowTechnique(void)
{
	delete m_pScreenGrid;
}

void IShadowTechnique::_Init( GLuint _texid )
{
	m_iTexID = _texid;
	m_pScreenGrid = new ScreenGrid( m_cDefaultRes );
}

void IShadowTechnique::SetResolution( float _res )
{
	m_pScreenGrid->SetResolution( _res ); 
}

float IShadowTechnique::GetResolution()
{
	return m_pScreenGrid->GetResolution();
}

const char* IShadowTechnique::GetName()
{
	return m_sName;
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

