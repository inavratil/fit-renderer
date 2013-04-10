#ifndef _FBOMANAGER_H_
#define _FBOMANAGER_H_

#include "globals.h"
#include "Singleton.h"

#define FBO_READ GL_READ_FRAMEBUFFER
#define FBO_DRAW GL_DRAW_FRAMEBUFFER
#define FBO_BOTH GL_FRAMEBUFFER

class FBOManager : public Singleton<FBOManager>
{ 

//-----------------------------------------------------------------------------
//-- Member variables

protected:
	///associative array with all FBOs
	map<string,GLuint> m_fbos;
    ///iterator for fbos container
    map<string,GLuint>::iterator m_ifbo;

	//-- ID of lastly bound FBO
	GLuint			m_lastFBO;


//-----------------------------------------------------------------------------
//-- Public methods

public:
	FBOManager(void);
	virtual ~FBOManager(void);

	void Add( const char* _name, GLuint _id );
	GLuint Get( const char* _name );

//-----------------------------------------------------------------------------
//-- Static methods

public:

	static bool CheckFBO();
	
	GLuint CreateFBO();
	GLuint CreateFBOAndAttachTexture( GLuint _tex, GLenum _target = FBO_BOTH );
	//TODO: pro vice FBO? a textur?
	//static GLuint CreateAndAttachNum( GLuint _tex, GLenum _target = FBO_BOTH );

	GLuint BindBuffer( GLuint _fbo );
	void UnbindBuffer();
};

typedef FBOManager* FBOManagerPtr;

#endif

