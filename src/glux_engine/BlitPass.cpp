#include "BlitPass.h"

#include "resources/FBOManager.h"

//-----------------------------------------------------------------------------

BlitPass::BlitPass() :
	m_tex_read( 0 ),
	m_tex_draw( 0 )
{
	_Init();
}

//-----------------------------------------------------------------------------

BlitPass::BlitPass( GLuint _read_tex, GLuint _draw_tex ) :
	m_tex_read( _read_tex ),
	m_tex_draw( _draw_tex )
{
	_Init();
	UpdateBounds();
}

//-----------------------------------------------------------------------------

BlitPass::~BlitPass(void)
{
	//-- delete read and draw texture names
	glDeleteTextures( 1, &m_tex_read );
	glDeleteTextures( 1, &m_tex_draw );
	//-- delete read and draw FBO names
	glDeleteFramebuffers( 1, &m_fbo_read );
	glDeleteFramebuffers( 1, &m_fbo_draw );
	
}

//-----------------------------------------------------------------------------

void BlitPass::_Init()
{
	//-- generates ids for FBOs
	m_fbo_read = m_FBOManager->CreateFBO();
	m_fbo_draw = m_FBOManager->CreateFBO();
	
	//-- set default mask
	m_mask = GL_COLOR_BUFFER_BIT;
	//-- set default filter
	m_filter = GL_NEAREST;
}

//-----------------------------------------------------------------------------

void BlitPass::Activate()
{	
	//-- bind read FBO and attach texture
	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo_read );
	m_FBOManager->AttachTexture( m_fbo_read, m_tex_read );	
	//-- bind draw FBO and attach texture
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo_draw );
	m_FBOManager->AttachTexture( m_fbo_draw, m_tex_draw );	
}

//-----------------------------------------------------------------------------

void BlitPass::Render()
{
	//-- do blit
	glBlitFramebuffer(
		m_src_bounds.x, m_src_bounds.y,		//-- source lower-left corner
		m_src_bounds.z, m_src_bounds.w,		//-- source upper-right corner
		m_dst_bounds.x, m_dst_bounds.y,		//-- destination lower-left corner
		m_dst_bounds.z, m_dst_bounds.w,		//-- destinatin upper-right corner
		m_mask,								//-- bit mask
		m_filter							//-- filter
		);
}

//-----------------------------------------------------------------------------

void BlitPass::Deactivate()
{
	//-- unbind read and draw FBOs
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}

//-----------------------------------------------------------------------------

void BlitPass::AttachReadTexture( GLuint _tex )
{
	m_tex_read = _tex;
	UpdateBounds();
}

//-----------------------------------------------------------------------------

void BlitPass::AttachDrawTexture( GLuint _tex )
{
	m_tex_draw = _tex;
	UpdateBounds();
}

//-----------------------------------------------------------------------------

void BlitPass::UpdateBounds()
{
	//-- initialize variables
	m_src_bounds = glm::ivec4( 0 );
	m_dst_bounds = glm::ivec4( 0 );

	//-- get read texture size to set bounds
	glBindTexture( GL_TEXTURE_2D, m_tex_read );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_src_bounds.z);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_src_bounds.w);
	glBindTexture( GL_TEXTURE_2D, 0 );

	//-- get draw texture size to set bounds
	glBindTexture( GL_TEXTURE_2D, m_tex_draw );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_dst_bounds.z);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_dst_bounds.w);
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//-----------------------------------------------------------------------------
