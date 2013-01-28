#ifndef _BILINEARWARPEDSHADOW_H_
#define _BILINEARWARPEDSHADOW_H_

#include "IShadowTechnique.h"

class BilinearWarpedShadow :
	public IShadowTechnique
{
protected:

public:
	BilinearWarpedShadow();
	~BilinearWarpedShadow(void);

	virtual bool Initialize(TLight* _light);
	virtual void PreRender();
	virtual void PostRender();

	virtual glm::vec2 ComputeDiff( glm::vec2 _P );

private:
	void _Init();
};

#endif
