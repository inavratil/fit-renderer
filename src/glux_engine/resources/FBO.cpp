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

void FBO::AttachColorTexture( GLuint _tex, unsigned _attachment )
{
	Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+_attachment, _tex, 0);
	Unbind();
}

//-----------------------------------------------------------------------------

void FBO::AttachDepthTexture( GLuint _tex )
{
	Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _tex, 0);
	Unbind();
}

//-----------------------------------------------------------------------------

void FBO::AttachDepthBuffer( unsigned _mode )
{
	//-- setup depth buffers
	GLuint depth;
    if( _mode > FBO_NO_DEPTH )
    {
        //-- create renderbuffers
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resX, resY);
    }
	if( _mode > FBO_NO_DEPTH )
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depth);
}

//-----------------------------------------------------------------------------

bool FBO::CheckStatus()
{
	Bind();
    //check FBO creation
    GLenum FBOstat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstat != GL_FRAMEBUFFER_COMPLETE)
    {
        if(FBOstat ==  GL_FRAMEBUFFER_UNDEFINED) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_UNDEFINED");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        if(FBOstat ==  GL_FRAMEBUFFER_UNSUPPORTED) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_UNSUPPORTED");
        return false;
    }
	Unbind();
    return true;
}