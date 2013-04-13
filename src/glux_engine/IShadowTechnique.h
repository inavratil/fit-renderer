#ifndef _ISHADOWTECHNIQUE_H_
#define _ISHADOWTECHNIQUE_H_

#include "light.h"
#include "ScreenGrid.h"
#include "tmaterial.h"
#include "shaderGen/ShaderFeature.h"

class ScreenGrid;

class IShadowTechnique
{
protected:
	
	static const int m_cDefaultRes = 128;

	const char*			m_sName;
	const char*			m_sDefines;
	bool				m_bEnabled;

	GLuint			m_iTexID;
	ScreenGrid*		m_pScreenGrid;
	ShaderFeature*	m_pShaderFeature;

public:
	IShadowTechnique();
	IShadowTechnique( GLuint _texid );
	virtual ~IShadowTechnique(void);

	//-- Virtual methods

	virtual bool Initialize(TLight* _light) = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;

	virtual glm::vec2 ComputeDiff( glm::vec2 _P ) = 0;

	virtual void SetResolution( float _res );
	virtual float GetResolution();
	
	//-- Set/Get methods
	
	const char* GetName();
	const char* GetDefines();
	void Enable();
	void Disable();
	bool IsEnabled();

	GLuint GetTexId();
	void SetTexId( GLuint _texid);

	ScreenGrid* GetGrid();
	
	void SetShaderFeature( ShaderFeature* _pFeature );
	ShaderFeature* GetShaderFeature();

	//-- Miscellaneous

	void UpdateGridRange( glm::vec4 _range );
	glm::vec4 GetGridRange();
	void GenerateGrid();
	void DrawGrid();

private:
	void _Init( GLuint _texid );
	void _Destroy();
};

///////////////////////////////////////////////////////////////////////////////
//-- Inline 


#endif
