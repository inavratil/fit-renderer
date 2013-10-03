#ifndef _BILINEARWARPEDSHADOW_H_
#define _BILINEARWARPEDSHADOW_H_

#include "IShadowTechnique.h"

class BilinearWarpedShadow :
	public IShadowTechnique
{
protected:
	enum Diff { D_X, D_Y };

	float*		m_pFuncValues;

public:
	BilinearWarpedShadow( TScene* _scene );
	~BilinearWarpedShadow(void);

	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

	//-- _P je v intervalu [0..res-1]
	virtual glm::vec2 ComputeDiff( glm::vec2 _P );

private:
	void _Init();

	//-- vrati 4 funkcni hodnoty lezi v rozich dane bunky mrizky. Druhy parametr urcuje, zda-li vraci x-ovy nebo y-ovy posun
	glm::vec4 _GetFuncValuesFromCell( glm::vec2 _c, int _d = D_X );
};

#endif
