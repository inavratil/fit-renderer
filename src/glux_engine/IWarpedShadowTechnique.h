/**
Class IWarpedShadowTechnique
======================

Abstraktni trida pro MOJE stinovaci techniky verze 2. Tedy ty, ktere pracuji s mrizkou.


*/
#ifndef _IWARPEDSHADOWTECHNIQUE_H_
#define _IWARPEDSHADOWTECHNIQUE_H_

#include "globals.h"
#include "sdk/RenderListener.h"
#include "shaderGen/ShaderFeature.h"
#include "light.h"
#include "ScreenGrid.h"

class ScreenGrid;

class IWarpedShadowTechnique : public RenderListener
{
protected:
	
	static const int m_cDefaultRes = 128;

	const char*			m_sName;
	const char*			m_sDefines;
	bool				m_bEnabled;

	GLuint			m_iTexID;
	ScreenGrid*		m_pScreenGrid;
	ShaderFeature*	m_pShaderFeature;
	TLight*			m_pLight;			//-- pointer to the light associated with the shadow technique

public:
	IWarpedShadowTechnique( TScene* _scene );
	virtual ~IWarpedShadowTechnique(void);

	//-----------------------------------------------------------------------------
	//-- Virtual methods

	//virtual bool Initialize() = 0;
	//virtual void PreRender() = 0;
	//virtual void PostRender() = 0;

	virtual glm::vec2 ComputeDiff( glm::vec2 _P ) = 0;

	//-- Nastavi rozliseni mrizky
	virtual void SetControlPointsCount( float _res );
	//-- Vrati rozliseni mrizky
	virtual float GetControlPointsCount();
	
	//-----------------------------------------------------------------------------
	//-- Set/Get methods
	
	const char* GetName();
	const char* GetDefines();
	void Enable();
	void Disable();
	bool IsEnabled();

	GLuint GetTexId();
	void SetTexId( GLuint _texid);

	TLight* GetLight();
	void SetLight( TLight* _light );

	ScreenGrid* GetGrid();
	
	void SetShaderFeature( ShaderFeature* _pFeature );
	ShaderFeature* GetShaderFeature();

	//-----------------------------------------------------------------------------
	//-- Miscellaneous

	void UpdateGridRange( glm::vec4 _range );
	glm::vec4 GetGridRange();
	void GenerateGrid();
	void DrawGrid();

private:
	void _Init();
	void _Destroy();
};

///////////////////////////////////////////////////////////////////////////////
//-- Inline 


#endif
