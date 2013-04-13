#ifndef _CUSTOMSHADER_H_
#define _CUSTOMSHADER_H_

#include "Material.h"

class TCustomShader : public Material
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	TCustomShader( const char* _name, unsigned _id );
	virtual ~TCustomShader(void);


};

typedef TCustomShader* CustomShaderPtr;

#endif

