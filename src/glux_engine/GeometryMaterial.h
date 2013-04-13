#ifndef _GEOMETRYMATERIAL_H_
#define _GEOMETRYMATERIAL_H_

#include "Material.h"

class GeometryMaterial : public Material
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

