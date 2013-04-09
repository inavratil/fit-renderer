#ifndef _GEOMETRYMATERIAL_H_
#define _GEOMETRYMATERIAL_H_

#include "Shader.h"

class GeometryMaterial : public Shader
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	GeometryMaterial(const char* _name, unsigned _id);
	virtual ~GeometryMaterial(void);


};

#endif

