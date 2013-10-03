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
	glm::mat4	Mcr;

	float*		m_pFuncValues;

public:
	SplineWarpedShadow( ScenePtr _scene );
	~SplineWarpedShadow(void);
	
	//-----------------------------------------------------------------------------
	//-- Virtual methods

	// MUSI BYT IMPLEMENTOVANA
	//-- Ciste virtualni metody tridy RenderListener (rodic tridy IShadowTechnique)
	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

	// MUSI BYT IMPLEMENTOVANA
	//-- Ciste virtualni metoda tridy IShadowTechnique
	//-- _P je v intervalu [0..res-1]
	virtual glm::vec2 ComputeDiff( glm::vec2 _P );
	//-- Virtualni metoda tridy IShadowTechnique
	virtual float GetControlPointsCount();

//-----------------------------------------------------------------------------
//-- Private methods

private:
	void _Init();

	//-- vrati 4 funkcni hodnoty lezi v rozich dane bunky mrizky. Druhy parametr urcuje, zda-li vraci x-ovy nebo y-ovy posun
	Splines4x4 _GetSplinesValues( glm::vec2 _c );
};

#endif
