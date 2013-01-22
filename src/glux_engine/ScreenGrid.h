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

	glm::vec4 m_v4Range;

	//-- Step values

	float m_fStepX;
	float m_fStepY;

	glm::vec2 m_v2Step;

	//-- Offset values

	float m_fOffsetX;
	float m_fOffsetY;

	glm::vec2 m_v2Offset;
	
	//-- Miscellaneous 

	int m_iNumLines;
	int m_iResolution;

public:
	ScreenGrid( );
	ScreenGrid( float _left, float _right, float _bottom, float _top );
	ScreenGrid( glm::vec4 _range );
	ScreenGrid( float _size );

	virtual ~ScreenGrid(void);

	//-- Set/Get member variables

	void SetRange( float _range ){ SetRange( glm::vec4(0,_range,0,_range) ); }
	void SetRange( glm::vec4 _range ){ m_fLeft = _range.x; m_fRight = _range.y; m_fBottom = _range.z; m_fTop = _range.w; }
	glm::vec4 GetRange(){ return glm::vec4(m_fLeft, m_fRight, m_fBottom, m_fTop); }

	void SetResolution( int _res ){ m_iResolution = _res; }
	int GetResolution(){ return m_iResolution; }

	//-- Miscellaneous

	void Draw(TMaterial* _mat);

};

#endif

