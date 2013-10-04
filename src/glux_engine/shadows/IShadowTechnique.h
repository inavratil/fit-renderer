#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

#include "sdk/RenderListener.h"
#include "scene.h"
#include "light.h"

class IShadowTechnique : public RenderListener
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	bool			m_bEnabled;
	TLight*			m_pLight;			//-- pointer to the light associated with the shadow technique


//-----------------------------------------------------------------------------
//-- Public methods 

public:
	IShadowTechnique( TScene* _scene );
	virtual ~IShadowTechnique(void);

	//-- MUST DEFINE THESE METHODS IN DERIVED CLASS
	//virtual bool Initialize() = 0;
	//virtual void PreRender() = 0;
	//virtual void PostRender() = 0;

	void Enable();
	void Disable();
	bool IsEnabled();

	TLight* GetLight();
	void SetLight( TLight* _light );
//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

