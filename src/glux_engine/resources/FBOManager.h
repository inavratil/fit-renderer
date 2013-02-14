#ifndef _FBOMANAGER_H_
#define _FBOMANAGER_H_

#include "globals.h"

class FBOManager
{ 

//-- Member variables

protected:
	static FBOManager * m_pInstance;

	///associative array with all FBOs
	map<string,GLuint> m_fbos;
    ///iterator for fbos container
    map<string,GLuint>::iterator m_ifbo;

//-- Public methods

public:
	FBOManager(void);
	virtual ~FBOManager(void);

	static FBOManager * Instance();
	static void Destroy();

	void Add( const char* _name, GLuint _id );
	GLuint Get( const char* _name );


};

#endif

