#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

class IShadowTechnique
{

public:
	IShadowTechnique();
	virtual ~IShadowTechnique(void);

	virtual void Initialize() = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

};

#endif

