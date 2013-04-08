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
	MipmapPass(void);
	virtual ~MipmapPass(void);
	
	virtual void Render();

};

#endif

