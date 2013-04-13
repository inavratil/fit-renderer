#ifndef _SCREENSPACEMATERIAL_H_
#define _SCREENSPACEMATERIAL_H_

#include "Material.h"

class ScreenSpaceMaterial : public Material
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	ScreenSpaceMaterial( const char* _name, unsigned _id );
	virtual ~ScreenSpaceMaterial(void);


};

typedef ScreenSpaceMaterial* ScreenSpaceMaterialPtr;

#endif

