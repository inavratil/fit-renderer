#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

#include "light.h"

class IShadowTechnique
{
protected:
	GLuint m_iTexID;

public:
	IShadowTechnique();
	IShadowTechnique( GLuint _texid ) : m_iTexID(_texid){};
	virtual ~IShadowTechnique(void);

	virtual bool Initialize(TLight* _light) = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

	virtual glm::vec2 ComputeDiff( glm::vec2 _P ) = 0;
	virtual float GetResolution() = 0;

	GLuint GetTexId(){ return m_iTexID; }
	void SetTexId( GLuint _texid){ m_iTexID = _texid; }
};

#endif
