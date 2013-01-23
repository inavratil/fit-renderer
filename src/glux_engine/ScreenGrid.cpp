#include "ScreenGrid.h"

ScreenGrid::ScreenGrid( float _res )
{
	ScreenGrid( 0.0, _res, 0.0, _res);
}

ScreenGrid::ScreenGrid( glm::vec4 _range )
{
	ScreenGrid( _range.x, _range.y, _range.z, _range.w);
}

ScreenGrid::ScreenGrid( float _left, float _right, float _bottom, float _top )
	: m_iNumLines(0), m_iResolution(1)
{
	UpdateRange( _left, _right, _bottom, _top );

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