#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include "tmaterial.h"

class MaterialManager
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	MaterialManager(void);
	virtual ~MaterialManager(void);

	MaterialPtr	Create( const char* _name );
};

typedef MaterialManager* MaterialManagerPtr;

#endif

