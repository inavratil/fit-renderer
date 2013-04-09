#ifndef _CUSTOMSHADER_H_
#define _CUSTOMSHADER_H_

#include "Shader.h"

class TCustomShader : public Shader
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

