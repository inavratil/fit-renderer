#ifndef _SCREENGRID_H_
#define _SCREENGRID_H_

#include "globals.h"
#include "resources/SceneManager.h"
#include "tmaterial.h"
#include "IShadowTechnique.h"

class IShadowTechnique;

class ScreenGrid
{
protected:
	
	static const int m_cDefaultRes = 128;

	//-- Range and associate members

	glm::vec4 m_vRange;
	glm::vec2 m_vStep;
	glm::vec2 m_vOffset;
	glm::vec2 m_vSize;

	//-- Miscellaneous 

	int m_iNumLines;
	int m_iResolution;

	TMaterial*		m_pMat;

public:
	ScreenGrid( glm::vec4 _range, float _res );
	ScreenGrid( float _res = 1.0 );

	virtual ~ScreenGrid(void);

	//-- Set/Get member variables

	inline void UpdateRange( float _range );
	inline void UpdateRange( glm::vec4 _range );
	inline glm::vec4 GetRange();
	inline glm::vec4 GetRangeAsOriginStep();
	inline glm::vec2 GetOffset();

	inline void SetResolution( int _res );
	inline int GetResolution();

	inline void ResetCounter();
	inline void IncCounter();
	
	//-- Miscellaneous

	void Draw();
	void InitVertexData();
	void GenerateGrid( IShadowTechnique* _pShadowTech );


private:
	void _Init( glm::vec4 _range, float _res );

	float _Left();
	float _Right();
	float _Bottom();
	float _Top();

};

///////////////////////////////////////////////////////////////////////////////
//-- Inlines

inline void ScreenGrid::UpdateRange( float _range )
{ 
	UpdateRange( glm::vec4(0,_range,0,_range) ); 
}

inline void ScreenGrid::UpdateRange( glm::vec4 _range )
{ 
	m_vRange = _range;

	m_vSize = glm::vec2( _Right() - _Left(), _Top() - _Bottom() );

	//FIXME: Opravdu tam musi byt -1.0 ?? Pro rozliseni 4 jsou tam tri bunky
	m_vStep = m_vSize/(float)(m_iResolution - 1);

	m_vOffset = 0.5f * m_vStep;

}

inline glm::vec4 ScreenGrid::GetRange()
{ 
	return m_vRange; 
}

inline glm::vec4 ScreenGrid::GetRangeAsOriginStep()
{
	return glm::vec4(_Left(), m_vStep.x, _Bottom(), m_vStep.y);
}

inline glm::vec2 ScreenGrid::GetOffset()
{
	return m_vOffset;
}

inline void ScreenGrid::SetResolution( int _res )
{ 
	m_iResolution = _res; 

	UpdateRange( m_vRange );
}

inline int ScreenGrid::GetResolution()
{ 
	return m_iResolution; 
}

inline void ScreenGrid::ResetCounter()
{
	m_iNumLines = 0;
}

inline void ScreenGrid::IncCounter()
{
	m_iNumLines++;
}

inline float ScreenGrid::_Left()
{
	return m_vRange.x;
}

inline float ScreenGrid::_Right()
{
	return m_vRange.y;
}

inline float ScreenGrid::_Bottom()
{
	return m_vRange.z;
}

inline float ScreenGrid::_Top()
{
	return m_vRange.w;
}

#endif

