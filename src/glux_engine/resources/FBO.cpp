#include "FBO.h"

//-----------------------------------------------------------------------------

FBO::FBO( unsigned _width, unsigned _height ) :
	m_width( _width ),
	m_height( _height ),
	m_depthbuffer( 0 ),
	m_includedBuffers( 0 )
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
	//-- init FBO's id
	glGenFramebuffers( 1, &m_id );

	//-- init viewport size
	m_viewport = glm::ivec4( 0 );
	m_viewport.z = m_width;
	m_viewport.w = m_height;
	m_last_viewport = glm::ivec4( 0 );

	//-- clear attachment list
	m_color_attachments.clear();

	//-- init pointer to depth texture
	m_depth_attachment = NULL;
}

//-----------------------------------------------------------------------------

void FBO::Destroy()
{
	glDeleteFramebuffers( 1, &m_id );
	m_id = 0;
	glDeleteRenderbuffers( 1, &m_depthbuffer );
	m_depthbuffer = 0;
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

	//-- set draw buffers according to number of color attachments
	if( m_color_attachments.size() )
	{
		vector<GLenum> list_att;
		for( m_it_ca = m_color_attachments.begin(); 
			m_it_ca != m_color_attachments.end();
			m_it_ca++
			)
			list_att.push_back( GL_COLOR_ATTACHMENT0 + m_it_ca->first );
		glDrawBuffers( list_att.size(), &list_att[0] );
	}

	//-- set viewport
	glGetIntegerv( GL_VIEWPORT, glm::value_ptr( m_last_viewport ) );
	glViewport( m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w );

	//-- clear attached buffers
	glClear( m_includedBuffers );

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

	//-- restore viewport setting
	glViewport( m_last_viewport.x, m_last_viewport.y, m_last_viewport.z, m_last_viewport.w );
	m_last_viewport = glm::ivec4( 0 );

	//-- restore draw buffers to some default value
	GLenum mrt[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers( 1, mrt );
}

//-----------------------------------------------------------------------------

void FBO::AttachColorTexture( TexturePtr _tex, int _attachment )
{
	GLuint tex_id = _tex->GetID();
	GLenum tex_target = _tex->GetTarget(); 

	Bind();
	if( tex_target == TEX_2D )
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+_attachment, tex_id, 0);
	else if( tex_target == TEX_2D_ARRAY )
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+_attachment, tex_id, 0, 0);
	Unbind();

	m_includedBuffers |= GL_COLOR_BUFFER_BIT;
	//-- assign the texture to the list
	m_color_attachments[_attachment] = _tex;
}

//-----------------------------------------------------------------------------

void FBO::UpdateColorLayer( int _index )
{
	Bind();
	for( m_it_ca = m_color_attachments.begin(); 
		m_it_ca != m_color_attachments.end();
		m_it_ca++
		)
	{
		if( m_it_ca->second->GetTarget() == TEX_2D_ARRAY )
			glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+m_it_ca->first, m_it_ca->second->GetID(), 0, _index  );
	}
	Unbind();
}

//-----------------------------------------------------------------------------

void FBO::UpdateDepthLayer( int _index )
{
	Bind();
	if( m_depth_attachment )
		if( m_depth_attachment->GetTarget() == TEX_2D_ARRAY )
			glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_attachment->GetID(), 0, _index  );
	Unbind();
}

//-----------------------------------------------------------------------------

void FBO::AttachDepthTexture( TexturePtr _tex )
{
	if( m_depthbuffer )
	{
		Bind();
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT ,GL_RENDERBUFFER, 0 );
		Unbind();
		glDeleteRenderbuffers( 1, &m_depthbuffer );
		m_depthbuffer = 0;
	}

	GLuint tex_id = _tex->GetID();
	GLenum tex_target = _tex->GetTarget(); 

	Bind();
	if( tex_target == TEX_2D )
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_id, 0);
	else if( tex_target == TEX_2D_ARRAY )
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_id, 0, 0);
	Unbind();

	m_includedBuffers |= GL_DEPTH_BUFFER_BIT;

	m_depth_attachment = _tex;
}

//-----------------------------------------------------------------------------

void FBO::AttachDepthBuffer( unsigned _mode )
{
	//-- check if depth buffer is attached
	if( m_depthbuffer || m_depth_attachment )
	{
		cerr << "WARNING (FBO): Depth buffer or texture already attached." << endl; 
		return;
	}

	//-- setup depth buffers
    if( _mode > FBO_NO_DEPTH )
    {
        //-- create renderbuffers
        glGenRenderbuffers( 1, &m_depthbuffer );
        glBindRenderbuffer( GL_RENDERBUFFER, m_depthbuffer );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height );
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );

		Bind();
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT ,GL_RENDERBUFFER, m_depthbuffer );
		Unbind();

		m_includedBuffers |= GL_DEPTH_BUFFER_BIT;
    }        
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