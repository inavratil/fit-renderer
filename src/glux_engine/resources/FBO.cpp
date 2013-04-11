#include "FBO.h"

//-----------------------------------------------------------------------------

FBO::FBO(void)
{
	Init();
}

//-----------------------------------------------------------------------------

FBO::~FBO(void)
{
	Destroy();
}

//-----------------------------------------------------------------------------

void FBO::Init()
{
	glGenFramebuffers( 1, &m_id );
}

//-----------------------------------------------------------------------------

void FBO::Destroy()
{
	glDeleteFramebuffers( 1, &m_id );
	m_id = 0;
}

//-----------------------------------------------------------------------------

GLuint FBO::Bind( GLenum _target )
{
	//-- save currently bound FBO
	int tmp;
	if( _target == FBO_READ )
		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &tmp );
	else if (_target == FBO_DRAW )
		glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &tmp );
	else
		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &tmp );
	
	m_lastFBO = tmp;
	//-- we don't need to bind the same FBO
	if( m_lastFBO != m_id )
		glBindFramebuffer( _target, m_id );

	return m_lastFBO;
	
}

//-----------------------------------------------------------------------------

void FBO::Unbind( GLenum _target )
{
	GLint tmp;
	if( _target == FBO_READ )
		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &tmp );
	else if (_target == FBO_DRAW )
		glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &tmp );
	else
		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &tmp );
	if (tmp != m_id) return;
	
	glBindFramebuffer( _target, m_lastFBO );
	m_lastFBO = 0;
}

//-----------------------------------------------------------------------------

void FBO::AttachTexture( GLuint _tex, unsigned _attachment )
{
	Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+_attachment, _tex, 0);
	Unbind();
}
