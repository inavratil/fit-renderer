#include "Pass.h"

//-----------------------------------------------------------------------------

Pass::Pass(void) :
	m_valid( false ),
	m_material( 0 )
{
}

//-----------------------------------------------------------------------------

Pass::~Pass(void)
{
}

//-----------------------------------------------------------------------------

void Pass::Render()
{
	if( !m_material ) return;

    //render material and quad covering whole screen
    m_material->RenderMaterial();
    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	m_material->DectivateTextures();	
}

//-----------------------------------------------------------------------------
