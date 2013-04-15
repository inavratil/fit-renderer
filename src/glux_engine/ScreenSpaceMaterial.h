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
	ScreenSpaceMaterial( const char* _name, const char* _vs, const char* _fs );
	virtual ~ScreenSpaceMaterial(void);

//-----------------------------------------------------------------------------
//-- Private methods 
	void _Init();

};

typedef ScreenSpaceMaterial* ScreenSpaceMaterialPtr;

#endif

