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

	bool CustomShader(TShader *vertex, TShader *tess_control, TShader *tess_eval, TShader *geometry, TShader *fragment);

//-----------------------------------------------------------------------------
//-- Private methods 
	void _Init();

};

typedef ScreenSpaceMaterial* ScreenSpaceMaterialPtr;

#endif

