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
		cerr<<"WARNING: no fbo with name "<<_name<<" ("<<__FILE__<<", "<<__LINE__<<").\n";
		return 0;
	}

	return m_fbos[_name];
}

//-----------------------------------------------------------------------------

GLuint FBOManager::GetID( const char* _name )
{
	if( m_fbos.find(_name) == m_fbos.end() )
	{
		cerr<<"WARNING: no fbo with name "<<_name<<" ("<<__FILE__<<", "<<__LINE__<<").\n";
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
FBOPtr FBOManager::CreateFBO( const char* _name )
{
	//-- create
	FBOPtr fbo = new FBO();

	//-- setup name
	string name = _name;
	if( name.empty() )
		name = "FBO"+num2str(m_fbos.size());
	
	//-- add the FBO to th list
	Add( name.c_str(), fbo );

	return fbo;
}

//-----------------------------------------------------------------------------

GLuint FBOManager::BindBuffer( const char* _name, GLenum _target )
{
	if( m_fbos.find(_name) == m_fbos.end() )
	{
		cerr<<"WARNING: no fbo with name "<<_name<<" ("<<__FILE__<<", "<<__LINE__<<").\n";
		return 0;
	}

	return m_fbos[_name]->Bind( _target );
}

//-----------------------------------------------------------------------------

void FBOManager::UnbindBuffer( const char* _name, GLenum _target )
{
	if( m_fbos.find(_name) == m_fbos.end() )
	{
		cerr<<"WARNING: no fbo with name "<<_name<<" ("<<__FILE__<<", "<<__LINE__<<").\n";
		return;
	}

	m_fbos[_name]->Unbind( _target );	
}
