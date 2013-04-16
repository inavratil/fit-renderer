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
	ScreenSpaceMaterial( const char* _name, const char* _vs, const char* _fs, const char* _vs_defines = "", const char* _fs_defines = "" );
	virtual ~ScreenSpaceMaterial(void);

	//bool CustomShader();
	virtual bool BakeMaterial(int light_count, int dpshadow_method = DPSM, bool use_pcf = true);

//-----------------------------------------------------------------------------
//-- Private methods 
	void _Init( const char* _name, const char* _vs, const char* _fs, const char* _vs_defines, const char* _fs_defines );

};

typedef ScreenSpaceMaterial* ScreenSpaceMaterialPtr;

#endif

