#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

#include "light.h"
#include "ScreenGrid.h"

class IShadowTechnique
{
protected:
	GLuint			m_iTexID;
	ScreenGrid*		m_pScreenGrid;

public:
	IShadowTechnique();
	IShadowTechnique( GLuint _texid );
	virtual ~IShadowTechnique(void);

	virtual bool Initialize(TLight* _light) = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

	virtual glm::vec2 ComputeDiff( glm::vec2 _P ) = 0;

	virtual void SetResolution( float _res );
	virtual float GetResolution();

	GLuint GetTexId();
	void SetTexId( GLuint _texid);

	void UpdateGridRange( glm::vec4 _range );
	glm::vec4 GetGridRange();
};

///////////////////////////////////////////////////////////////////////////////
//-- Inline 

inline void IShadowTechnique::SetResolution( float _res )
{
	m_pScreenGrid->SetResolution( _res );
}
inline float IShadowTechnique::GetResolution()
{
	return m_pScreenGrid->GetResolution();
}

inline GLuint IShadowTechnique::GetTexId()
{ 
	return m_iTexID; 
}

inline void IShadowTechnique::SetTexId( GLuint _texid)
{ 
	m_iTexID = _texid; 
}

#endif
