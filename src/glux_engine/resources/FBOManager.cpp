#include "FBOManager.h"

template<> FBOManager * Singleton<FBOManager>::m_pInstance = 0;

//-----------------------------------------------------------------------------

FBOManager::FBOManager(void)
{
}

//-----------------------------------------------------------------------------

FBOManager::~FBOManager(void)
{
	for( m_it_fbos=m_fbos.begin(); m_it_fbos!=m_fbos.end(); m_it_fbos++ )
		delete m_it_fbos->second;
}

//-----------------------------------------------------------------------------

void FBOManager::Add( const char* _name, FBOPtr _fbo )
{
	m_fbos[_name] = _fbo;
}

//-----------------------------------------------------------------------------

FBOPtr FBOManager::Get( const char* _name )
{
	if( m_fbos.find(_name) == m_fbos.end() )
	{
		cerr<<"WARNING (FBOManager): no fbo with name "<<_name<<"\n";
		return 0;
	}

	return m_fbos[_name];
}

//-----------------------------------------------------------------------------

GLuint FBOManager::GetID( const char* _name )
{
	if( m_fbos.find(_name) == m_fbos.end() )
	{
		cerr<<"WARNING (FBOManager): no fbo with name "<<_name<<"\n";
		return 0;
	}

	return m_fbos[_name]->GetID();
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Check framebuffer status
@return is FBO complete?
***************************************************************************************************/
bool FBOManager::CheckFBO()
{
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
    return true;
}

//-----------------------------------------------------------------------------
/*
    if( fbo_mode > NO_DEPTH )
    {
        //create renderbuffers
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resX, resY);
    }
	[...]
	if( fbo_mode > NO_DEPTH )
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depth);
*/
FBOPtr FBOManager::CreateFBO( const char* _name, unsigned _mode )
{
	FBOPtr fbo = new FBO();

	string name = _name;
	if( name.empty() )
		name = "FBO"+m_fbos.size();

	Add( name.c_str(), fbo );

	return fbo;
}

//-----------------------------------------------------------------------------

FBOPtr FBOManager::CreateFBOAndAttachTexture( const char* _name, GLuint _tex, GLenum _target )
{
	FBOPtr fbo = CreateFBO( _name );
		
	//-- bind FBO
	fbo->Bind();
	//-- attach texture to the frame buffer
	glFramebufferTexture2D(_target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);
	//-- check FBO status
	if(!CheckFBO())
		ShowMessage("ERROR: FBO creation failed!",false);
	//-- unbind FBO
	fbo->Unbind();

	return fbo;
}

//-----------------------------------------------------------------------------

GLuint FBOManager::BindBuffer( GLuint _fbo )
{
	//-- save currently bound FBO
	int tmp;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &tmp );
	m_lastFBO = tmp;
	//-- we don't need to bind the same FBO
	if( m_lastFBO != _fbo )
		glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

	return m_lastFBO;
}

//-----------------------------------------------------------------------------

void FBOManager::UnbindBuffer()
{
	//-- check, if some FBO is bound
	GLint tmp;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &tmp );
	m_lastFBO = 0;
	//-- if so, unbind it
	if( !tmp )
		glBindFramebuffer( GL_FRAMEBUFFER, m_lastFBO );
	
}

//-----------------------------------------------------------------------------

void FBOManager::AttachTexture( GLuint _fbo, GLuint _tex, unsigned _attachment )
{
	BindBuffer( _fbo );
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+_attachment, GL_TEXTURE_2D, _tex, 0);
	UnbindBuffer();
}