#include "IShadowTechnique.h"


IShadowTechnique::IShadowTechnique()
	: m_iTexID(0)
{
	m_pScreenGrid = new ScreenGrid(128.0);
}

IShadowTechnique::IShadowTechnique( GLuint _texid ) 
	: m_iTexID(_texid)
{
	m_pScreenGrid = new ScreenGrid(128.0);
}

IShadowTechnique::~IShadowTechnique(void)
{
	delete m_pScreenGrid;
}

void IShadowTechnique::UpdateGridRange( glm::vec4 _range )
{
	m_pScreenGrid->UpdateRange(_range);
}

glm::vec4 IShadowTechnique::GetGridRange()
{
	return m_pScreenGrid->GetRange();
}