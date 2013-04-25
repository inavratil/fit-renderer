#ifndef _SPLINEWARPEDSHADOW_H_
#define _SPLINEWARPEDSHADOW_H_

#include "IShadowTechnique.h"

struct Splines4x4 {
	glm::mat4x2 p0;
	glm::mat4x2 p1;
	glm::mat4x2 p2;
	glm::mat4x2 p3;
};

class SplineWarpedShadow :
	public IShadowTechnique
{
protected:
	enum Diff { D_X, D_Y };

	//-- inicializace po sloupcich
	glm::mat4 Mcr;

	float*		m_pFuncValues;

public:
	SplineWarpedShadow();
	~SplineWarpedShadow(void);

	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

	//-- _P je v intervalu [0..res-1]
	virtual glm::vec2 ComputeDiff( glm::vec2 _P );

	virtual float GetResolution();

private:
	void _Init();

	//-- vrati 4 funkcni hodnoty lezi v rozich dane bunky mrizky. Druhy parametr urcuje, zda-li vraci x-ovy nebo y-ovy posun
	Splines4x4 _GetSplinesValues( glm::vec2 _c );
};

#endif
