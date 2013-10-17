#ifndef _DPSHADOWMAP_H_
#define _DPSHADOWMAP_H_

#include "globals.h"
#include "IShadowTechnique.h"

class DPShadowMap : public IShadowTechnique
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	int			m_i_shadow_res;
	float		m_f_intensity;
	bool		m_b_use_pcf;


//-----------------------------------------------------------------------------
//-- Public methods 

public:
	DPShadowMap( TScene* _scene );
	virtual ~DPShadowMap(void);

	virtual bool Initialize();
	virtual void PreRender();
	virtual void PostRender();

	void SetShadowParams( int _res, int _intensity = 0.1f );

//-----------------------------------------------------------------------------
//-- Private methods

private:
	void _Init();
	void _Destroy();
	void _EvaluateBestConfiguration();
};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

