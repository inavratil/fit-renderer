#include "SimplePass.h"

//-----------------------------------------------------------------------------

SimplePass::SimplePass( unsigned _width, unsigned _height  ) :
	m_shader(""),
	m_activated( false )
{
	_Init( _width, _height );
}

//-----------------------------------------------------------------------------

SimplePass::~SimplePass(void)
{
	//-- delete texture's names
	for(int i=0; i<m_output_textures.size(); ++i)
		glDeleteTextures( 1, &m_output_textures[i].id );
	//-- delete FBO
	delete m_fbo;
}

//-----------------------------------------------------------------------------

void SimplePass::_Init( unsigned _width, unsigned _height )
{
	//-- setup FBO
	m_fbo = new FBO( _width, _height );

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