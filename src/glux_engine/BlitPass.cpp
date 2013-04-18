#include "BlitPass.h"

//-----------------------------------------------------------------------------

BlitPass::BlitPass( unsigned _width, unsigned _height ) :
	m_tex_read( 0 ),
	m_tex_draw( 0 )
{
	_Init( _width, _height );
}

//-----------------------------------------------------------------------------

BlitPass::~BlitPass(void)
{
	//-- delete read and draw texture names
	glDeleteTextures( 1, &m_tex_read );
	glDeleteTextures( 1, &m_tex_draw );

	//-- delete FBOs
	delete m_fbo_read;
	delete m_fbo_draw;
}

//-----------------------------------------------------------------------------

void BlitPass::_Init( unsigned _width, unsigned _height )
{
	//-- generates ids for FBOs
	m_fbo_read = new FBO( _width, _height );
	m_fbo_draw = new FBO( _width, _height );
	
	//-- set default mask
	m_mask = GL_COLOR_BUFFER_BIT;
	//-- set default filter
	m_filter = GL_NEAREST;
}

//-----------------------------------------------------------------------------

void BlitPass::Activate()
{	
	//-- bind read and draw FBO
	m_fbo_draw->Bind( FBO_DRAW );
	m_fbo_read->Bind( FBO_READ );

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
	m_fbo_read->Unbind( FBO_READ );
	m_fbo_draw->Unbind( FBO_DRAW );
}

//-----------------------------------------------------------------------------

bool BlitPass::Validate()
{
	m_valid = true;
	
	if(!m_fbo_read->CheckStatus())
    {
		cerr<<"ERROR (BlitPass): Pass validation failed due to error in FBO.\n";
		m_valid = false;
    }

	if(!m_fbo_draw->CheckStatus())
    {
		cerr<<"ERROR (BlitPass): Pass validation failed due to error in FBO.\n";
		m_valid = false;
    }

	return m_valid;
}

//-----------------------------------------------------------------------------

void BlitPass::AttachReadTexture( TexturePtr _tex )
{
	m_tex_read = _tex->GetID();
	//-- attach texture
	m_fbo_read->AttachColorTexture( _tex );
	UpdateBounds();
}

//-----------------------------------------------------------------------------

void BlitPass::AttachDrawTexture( TexturePtr _tex )
{
	m_tex_draw = _tex->GetID();
	//-- attach texture
	m_fbo_draw->AttachColorTexture( _tex );
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
