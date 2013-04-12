#ifndef _FBO_H_
#define _FBO_H_

#include "globals.h"

#define FBO_READ GL_READ_FRAMEBUFFER
#define FBO_DRAW GL_DRAW_FRAMEBUFFER
#define FBO_BOTH GL_FRAMEBUFFER

//-- FBO creation mode
enum FBOModes { FBO_NO_DEPTH, FBO_DEPTH_ONLY, FBO_DEPTH_AND_STENCIL };

class FBO
{ 

//-----------------------------------------------------------------------------
//-- Member variables

protected:
	GLuint			m_id;
	GLuint			m_lastFBO;	
	int				m_width;
	int				m_height;
	GLuint			m_depthbuffer;
	GLbitfield		m_includedBuffers;

	glm::ivec4		m_viewport;
	glm::ivec4		m_last_viewport;

	map<int, GLuint>			m_color_attachments;
	map<int, GLuint>::iterator	m_it_ca;
	
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	FBO( unsigned _width, unsigned _height );
	virtual ~FBO(void);

	void Init();
	void Destroy();

	//-- Set/Get id
	void SetID( GLuint _id ){ m_id = _id; }
	GLuint GetID(){ return m_id; }

	//-- Get width/height
	int GetWidth(){ return m_width; }
	int GetHeight(){ return m_height; }

	GLuint Bind( GLenum _target = FBO_BOTH );
	void Unbind( GLenum _target = FBO_BOTH );

	void AttachColorTexture( GLuint _tex, int _attachment = 0 );
	void AttachDepthTexture( GLuint _tex );
	void AttachDepthBuffer( unsigned _mode = FBO_DEPTH_ONLY );
	bool CheckStatus(); 

};

typedef FBO* FBOPtr;

#endif

