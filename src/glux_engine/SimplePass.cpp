#include "SimplePass.h"

//-----------------------------------------------------------------------------

SimplePass::SimplePass( FBOManagerPtr _fbo_manager ) :
	Pass( _fbo_manager ),
	m_shader(""),
	m_activated( false )
{
	_Init();
}

//-----------------------------------------------------------------------------
SimplePass::SimplePass( FBOManagerPtr _fbo_manager, GLuint _tex ) :
	Pass( _fbo_manager ),
	m_shader(""),
	m_activated( false )
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
	GLuint fbo = m_fbo->GetID();
	glDeleteFramebuffers( 1, &fbo );
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
	m_fbo->Bind();
	for(int i=0; i<m_output_textures.size(); ++i)
	{
		PassTexture t = m_output_textures[i];
		m_fbo->AttachColorTexture( t.id, t.pos );
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
	m_fbo->Unbind();
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