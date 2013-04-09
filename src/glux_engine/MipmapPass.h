#ifndef _MIPMAPPASS_H_
#define _MIPMAPPASS_H_

#include "globals.h"
#include "Pass.h"

class MipmapPass : public Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	GLuint			m_fbo;
	GLuint			m_tex;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	MipmapPass( GLuint _tex );
	virtual ~MipmapPass(void);
	
	virtual void Render();

};

#endif

