#include "ScreenGrid.h"

ScreenGrid::ScreenGrid( float _res )
{
	ScreenGrid( glm::vec4(0.0, _res, 0.0, _res), _res );
}

ScreenGrid::ScreenGrid( glm::vec4 _range, float _res )
	: m_iNumLines(0), m_iResolution(_res)
{
	UpdateRange( _range );

	TShader grid_vert("data/shaders/quad.vert", "");
	TShader grid_frag("data/shaders/warping/dbg_polynomials_grid.frag", "");
}


ScreenGrid::~ScreenGrid(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//-- Public methods

void ScreenGrid::Draw(TMaterial* _mat)
{
	if(!_mat) return;

	_mat->RenderMaterial();
	glBindVertexArray(SceneManager::Instance()->getVBO(VBO_ARRAY, "polynomials_grid"));
	glDrawArrays( GL_LINES, 0, m_iNumLines );
	glBindVertexArray( 0 );
}
