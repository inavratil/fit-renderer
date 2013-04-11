#ifndef _FBO_H_
#define _FBO_H_

#include "globals.h"

#define FBO_READ GL_READ_FRAMEBUFFER
#define FBO_DRAW GL_DRAW_FRAMEBUFFER
#define FBO_BOTH GL_FRAMEBUFFER

class FBO
{ 

//-----------------------------------------------------------------------------
//-- Member variables

protected:
	GLuint			m_id;
	GLuint			m_lastFBO;
	
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	FBO(void);
	virtual ~FBO(void);

	void Init();
	void Destroy();

	//-- Set/Get id
	void SetID( GLuint _id ){ m_id = _id; }
	GLuint GetID(){ return m_id; }

	GLuint Bind( GLenum _target = FBO_BOTH );
	void Unbind( GLenum _target = FBO_BOTH );

	void AttachTexture( GLuint _tex, unsigned _attachment = 0 );

};

typedef FBO* FBOPtr;

#endif

