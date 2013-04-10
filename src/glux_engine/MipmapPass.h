#ifndef _MIPMAPPASS_H_
#define _MIPMAPPASS_H_

#include "globals.h"
#include "Pass.h"

class MipmapPass : public Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	MipmapPass( GLuint _tex );
	virtual ~MipmapPass(void);
	
	virtual void Render();
	virtual void Activate();
	virtual void Deactivate();

};

#endif

