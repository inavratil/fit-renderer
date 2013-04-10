#include "Pass.h"

//-----------------------------------------------------------------------------

Pass::Pass( GLuint _tex ) :
	m_shader(""),
	m_activated( false )
{
	//-- assign texture
	AttachOutputTexture( 0, _tex );

	//-- setup FBO
	m_fbo = m_FBOManager->CreateFBO();

	//-- get max number of attachments in the system
	glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &m_max_attachments );
}

//-----------------------------------------------------------------------------

Pass::~Pass(void)
{
	//-- delete texture's names
	for(int i=0; i<m_output_textures.size(); ++i)
		glDeleteTextures( 1, &m_output_textures[i].id );
	//-- delete FBO's names
	glDeleteFramebuffers( 1, &m_fbo );
}

//-----------------------------------------------------------------------------

void Pass::Activate()
{
	m_FBOManager->BindBuffer( m_fbo );
	m_activated = true;
}

//-----------------------------------------------------------------------------

void Pass::Render()
{
	if( !m_activated ) return;

	for(int i=0; i<m_output_textures.size(); ++i)
	{
		PassTexture t = m_output_textures[i];
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+t.pos,GL_TEXTURE_2D, t.id, 0);
	}
}

//-----------------------------------------------------------------------------

void Pass::Deactivate()
{
	m_FBOManager->UnbindBuffer();
	m_activated = false;
}

//-----------------------------------------------------------------------------

void Pass::Process()
{
	Activate();
	Render();
	Deactivate();
}

//-----------------------------------------------------------------------------

void Pass::AttachOutputTexture( unsigned _pos, GLuint _tex )
{
	assert( m_output_textures.size() < m_max_attachments  );
	assert( _pos < m_max_attachments  );
	assert( _tex != 0 );

	m_output_textures.push_back( PassTexture( _pos, _tex ) );
}

//-----------------------------------------------------------------------------