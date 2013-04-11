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
	GLuint			m_target;
	GLuint			m_lastFBO;
	
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	FBO(void);
	FBO( GLenum _target );
	virtual ~FBO(void);

	void Init();
	void Destroy();

	//-- Set/Get id
	void SetID( GLuint _id ){ m_id = _id; }
	GLuint GetID(){ return m_id; }
	//-- Set/Get target
	void SetTarget( GLenum _target ){ m_target = _target; }
	GLenum GetTarget(){ return m_target; }

	GLuint Bind();
	void Unbind();

};

typedef FBO* FBOPtr;

#endif

