#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

#include "light.h"

class IShadowTechnique
{

public:
	IShadowTechnique();
	virtual ~IShadowTechnique(void);

	virtual bool Initialize(TLight* _light) = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

};

#endif

