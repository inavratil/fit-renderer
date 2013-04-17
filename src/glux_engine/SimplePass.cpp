#include "SimplePass.h"

//-----------------------------------------------------------------------------

SimplePass::SimplePass( unsigned _width, unsigned _height  ) :
	m_shader(""),
	m_activated( false ),
	m_depthbuffer_used( true )
{
	_Init( _width, _height );
}

//-----------------------------------------------------------------------------

SimplePass::~SimplePass(void)
{
	//-- delete FBO
	if( m_fbo )
		delete m_fbo;
	m_fbo = 0;
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

bool SimplePass::Validate()
{
	m_valid = true;
	
	//-- create depth renderbuffer
	if( m_depthbuffer_used )
	{	
		m_fbo->AttachDepthBuffer();
	}
	
	//-- attach color textures
	for(int i=0; i<m_output_textures.size(); ++i)
	{
		PassTexture t = m_output_textures[i];
		m_fbo->AttachColorTexture( t.tex, t.pos );
	}

	if(!m_fbo->CheckStatus())
    {
		cerr<<"ERROR (SimplePass): Pass validation failed due to error in FBO.\n";
		m_valid = false;
    }
	

	return m_valid;
}

//-----------------------------------------------------------------------------

void SimplePass::AttachOutputTexture( unsigned _pos, TexturePtr _tex )
{
	assert( m_output_textures.size() < m_max_attachments  );
	assert( _pos < m_max_attachments  );
	assert( _tex != 0 );

	m_output_textures.push_back( PassTexture( _pos, _tex ) );
}

//-----------------------------------------------------------------------------