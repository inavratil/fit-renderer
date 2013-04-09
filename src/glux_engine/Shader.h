#ifndef _SHADER_H_
#define _SHADER_H_

#include "globals.h"

class Shader
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	string				m_name;
    unsigned			m_id;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Shader( const char* _name, unsigned _id );
	virtual ~Shader(void);


};

#endif

