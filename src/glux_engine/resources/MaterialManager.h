#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include "globals.h"
#include "sdk/Singleton.h"
#include "sdk/Material.h"

class MaterialManager : public Singleton<MaterialManager>
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	    
	//-- associative array with all materials
    map<string,Material*> m_materials;
    //-- iterator for materials container
    map<string,Material*>::iterator m_im;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	MaterialManager(void);
	virtual ~MaterialManager(void);

//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

typedef MaterialManager* MaterialManagerPtr;

#endif

