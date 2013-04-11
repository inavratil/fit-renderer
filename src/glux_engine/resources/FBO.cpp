#include "FBO.h"

//-----------------------------------------------------------------------------

FBO::FBO(void) :
	m_target( GL_FRAMEBUFFER )
{
	Init();
}

//-----------------------------------------------------------------------------
FBO::FBO( GLenum _target ) :
	m_target( _target )
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

GLuint FBO::Bind()
{
	//-- save currently bound FBO
	int tmp;
	if( m_target == FBO_READ )
		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &tmp );
	else if ( m_target == FBO_DRAW )
		glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &tmp );
	else
		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &tmp );
	
	m_lastFBO = tmp;
	//-- we don't need to bind the same FBO
	if( m_lastFBO != m_id )
		glBindFramebuffer( m_target, m_id );

	return m_lastFBO;
	
}

//-----------------------------------------------------------------------------

void FBO::Unbind()
{
	GLint tmp;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &tmp );
	if (tmp != m_id) return;
	
	glBindFramebuffer( GL_FRAMEBUFFER, m_lastFBO );
	m_lastFBO = 0;
}
