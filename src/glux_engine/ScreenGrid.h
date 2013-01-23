#ifndef _SCREENGRID_H_
#define _SCREENGRID_H_

#include "globals.h"
#include "SceneManager.h"
#include "material.h"

class ScreenGrid
{
protected:
	
	//-- Range values

	float m_fLeft;
	float m_fRight;
	float m_fBottom;
	float m_fTop;

	glm::vec4 m_vRange;

	//-- Step values

	float m_fStepX;
	float m_fStepY;

	glm::vec2 m_vStep;

	//-- Offset values

	float m_fOffsetX;
	float m_fOffsetY;

	glm::vec2 m_vOffset;
	
	//-- Size

	float m_fWidth;
	float m_fHeight;
	glm::vec2 m_vSize;

	//-- Miscellaneous 

	int m_iNumLines;
	int m_iResolution;

public:
	ScreenGrid( float _left, float _right, float _bottom, float _top );
	ScreenGrid( glm::vec4 _range );
	ScreenGrid( float _res );

	virtual ~ScreenGrid(void);

	//-- Set/Get member variables

	inline void UpdateRange( float _range );
	inline void UpdateRange( glm::vec4 _range );
	inline void UpdateRange( float _left, float _right, float _bottom, float _top );
	inline glm::vec4 GetRange();

	void SetResolution( int _res ){ m_iResolution = _res; }
	int GetResolution(){ return m_iResolution; }

	//-- Miscellaneous

	void Draw(TMaterial* _mat);

};

inline void ScreenGrid::UpdateRange( float _range )
{ 
	UpdateRange( glm::vec4(0,_range,0,_range) ); 
}

inline void ScreenGrid::UpdateRange( glm::vec4 _range )
{
	UpdateRange( _range.x, _range.y, _range.z, _range.w );
} 

inline void ScreenGrid::UpdateRange( float _left, float _right, float _bottom, float _top )
{ 
	m_fLeft = _left; 
	m_fRight = _right; 
	m_fBottom = _bottom; 
	m_fTop = _top; 

	m_fWidth = m_fRight - m_fLeft;
	m_fHeight = m_fRight - m_fLeft;

}

inline glm::vec4 ScreenGrid::GetRange()
{ 
	return glm::vec4(m_fLeft, m_fRight, m_fBottom, m_fTop); 
}

#endif

