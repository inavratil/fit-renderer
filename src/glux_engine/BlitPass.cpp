#include "BlitPass.h"

#include "resources/FBOManager.h"

//-----------------------------------------------------------------------------

BlitPass::BlitPass( GLuint _tex_read, GLuint _tex_draw ) :
	Pass( _tex_draw )
{
	//-- assign textures to read from and draw to
	assert( _tex_read != 0 );
	m_tex_read = _tex_read;

	//-- initialize bounds
	_InitBounds();

	m_fbo_read = m_FBOManager->CreateFBOAndAttachTexture( m_tex_read, FBO_READ );
	
	//-- set default mask
	m_mask = GL_COLOR_BUFFER_BIT;
	//-- set default filter
	m_filter = GL_NEAREST;
}

//-----------------------------------------------------------------------------

BlitPass::~BlitPass(void)
{
	//-- delete read texture names
	glDeleteTextures( 1, &m_tex_read );
	//-- delete read FBO names
	glDeleteFramebuffers( 1, &m_fbo_read );
	
}

//-----------------------------------------------------------------------------

void BlitPass::Render()
{
	Pass::Render();

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

void BlitPass::Activate()
{
	Pass::Activate();

	//-- bind read and draw FBOs
	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo_read );
	
}

//-----------------------------------------------------------------------------

void BlitPass::Deactivate()
{
	//-- unbind read and draw FBOs
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

	Pass::Deactivate();
}

//-----------------------------------------------------------------------------

void BlitPass::_InitBounds()
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
	glBindTexture( GL_TEXTURE_2D, m_output_textures[0].id );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_dst_bounds.z);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_dst_bounds.w);
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//-----------------------------------------------------------------------------
