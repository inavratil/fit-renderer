#include "SimplePass.h"

//-----------------------------------------------------------------------------

SimplePass::SimplePass() :
	m_shader(""),
	m_activated( false )
{
	_Init();
}

//-----------------------------------------------------------------------------
SimplePass::SimplePass( GLuint _tex )
{
	_Init();
	AttachOutputTexture( 0, _tex );
}

//-----------------------------------------------------------------------------

SimplePass::~SimplePass(void)
{
	//-- delete texture's names
	for(int i=0; i<m_output_textures.size(); ++i)
		glDeleteTextures( 1, &m_output_textures[i].id );
	//-- delete FBO's names
	glDeleteFramebuffers( 1, &m_fbo );
}

//-----------------------------------------------------------------------------

void SimplePass::_Init()
{
	//-- setup FBO
	m_fbo = m_FBOManager->CreateFBO();

	//-- get max number of attachments in the system
	glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &m_max_attachments );
}

void SimplePass::Activate()
{
	assert(m_FBOManager);

	m_FBOManager->BindBuffer( m_fbo );
	for(int i=0; i<m_output_textures.size(); ++i)
	{
		PassTexture t = m_output_textures[i];
		m_FBOManager->AttachTexture (m_fbo, t.id, t.pos );
	}
	m_activated = true;
}

//-----------------------------------------------------------------------------

void SimplePass::Render()
{
	if( !m_activated ) return;
	
}

//-----------------------------------------------------------------------------

void SimplePass::Deactivate()
{
	m_FBOManager->UnbindBuffer();
	m_activated = false;
}

//-----------------------------------------------------------------------------

void SimplePass::AttachOutputTexture( unsigned _pos, GLuint _tex )
{
	assert( m_output_textures.size() < m_max_attachments  );
	assert( _pos < m_max_attachments  );
	assert( _tex != 0 );

	m_output_textures.push_back( PassTexture( _pos, _tex ) );
}

//-----------------------------------------------------------------------------