#include "SplineWarpedShadow.h"

#include "shadow.h"
#include "shaderGen/SFSplineWarpedShadow.h"

SplineWarpedShadow::SplineWarpedShadow()
{
	_Init();
}


SplineWarpedShadow::~SplineWarpedShadow(void)
{
}

void SplineWarpedShadow::_Init()
{
	m_sName = "Spline";
	m_sDefines = "#define SPLINE_WARP\n";

	m_pFuncValues = NULL;

	m_pShaderFeature = new SFSplineWarpedShadow();

	Mcr = 0.5 * glm::mat4(
		 0.0f,	2.0f,	0.0f,	0.0f,
		-1.0f,	0.0f,	1.0f,	0.0f,
		 2.0f, -5.0f,	4.0f,  -1.0f,
		-1.0f,	3.0f,	-3.0f,  1.0f
		);
}

bool SplineWarpedShadow::Initialize()
{
	return true;
}

void SplineWarpedShadow::PreRender()
{
	int res = (int)this->GetResolution();
	m_pFuncValues = new float[res*res*2];
	
	memset(m_pFuncValues, 0, res*res*2*sizeof(float));

	glBindTexture(GL_TEXTURE_2D, m_iTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, m_pFuncValues);
	glBindTexture(GL_TEXTURE_2D, 0 ); 

	this->GenerateGrid();

	delete [] m_pFuncValues;
	m_pFuncValues = NULL;
}

void SplineWarpedShadow::PostRender()
{
}

//-- _P je v intervalu [0..grid_res-1]
glm::vec2 SplineWarpedShadow::ComputeDiff( glm::vec2 _P )
{
	glm::vec2 diff( 0.0 );

	if( m_iTexID == 0 || m_pFuncValues == NULL || !m_bEnabled ) return diff;

	//-- vypocet souradnice bunky, ve ktere se bod _P nachazi
	glm::vec2 grid_coords = glm::floor( _P );
	
	glm::mat4 P;
	glm::vec4 Tx, Ty;
	glm::vec4 Q;

	Splines4x4 splines;

	//-- prevod do intervalu [0..1], tj. pozice v bunce
	float x = glm::fract(_P.x);
	float y = glm::fract(_P.y);

	splines = _GetSplinesValues( grid_coords );

	Tx = glm::vec4( 1.0, x, glm::pow(x, 2.0f), glm::pow(x,3.0f) );
	Ty = glm::vec4( 1.0, y, glm::pow(y, 2.0f), glm::pow(y, 3.0f) );

	//-- diff X
	
	//-- definice po sloupcich
	P = glm::mat4(
		splines.p0[0].x,	splines.p1[0].x,	splines.p2[0].x,	splines.p3[0].x,
		splines.p0[1].x,	splines.p1[1].x,	splines.p2[1].x,	splines.p3[1].x,
		splines.p0[2].x,	splines.p1[2].x,	splines.p2[2].x,	splines.p3[2].x,
		splines.p0[3].x,	splines.p1[3].x,	splines.p2[3].x,	splines.p3[3].x
		);

	Q = P * Mcr * Tx;
	diff.x = glm::dot(Q * Mcr, Ty);

	//-- diff Y

	//-- definice po sloupcich
	P = glm::mat4(
		splines.p0[0].y,	splines.p1[0].y,	splines.p2[0].y,	splines.p3[0].y,
		splines.p0[1].y,	splines.p1[1].y,	splines.p2[1].y,	splines.p3[1].y,
		splines.p0[2].y,	splines.p1[2].y,	splines.p2[2].y,	splines.p3[2].y,
		splines.p0[3].y,	splines.p1[3].y,	splines.p2[3].y,	splines.p3[3].y
		);

	Q = P * Mcr * Tx;
	glm::vec4 v = Mcr * Ty;
	diff.y = glm::dot(Q * Mcr, Ty);

	return diff * POLY_BIAS;
}

#define I(i,j) (2*( (j)*(res) + (i) ))
Splines4x4 SplineWarpedShadow::_GetSplinesValues( glm::vec2 _c )
{

	Splines4x4 ret;

	if( m_pFuncValues == NULL ) 
		return ret;

	//-- v tomto pridade se bere skutecne rozliseni, tj. rozliseni textury
	int res = (int)this->GetResolution();
	int x = (int)_c.x, y = (int)_c.y;
	assert( _c.x<res && _c.y<res );

	//-- jedna bunka
	// P3  ----
	//     |  |
	// P2  ----
	//     |XX|
	// P1  ----
	//     |  |
	// P0  ----

	memcpy( glm::value_ptr(ret.p0), m_pFuncValues + I(x,y + 0), sizeof(glm::mat4x2) );
	memcpy( glm::value_ptr(ret.p1), m_pFuncValues + I(x,y + 1), sizeof(glm::mat4x2) );
	memcpy( glm::value_ptr(ret.p2), m_pFuncValues + I(x,y + 2), sizeof(glm::mat4x2) );
	memcpy( glm::value_ptr(ret.p3), m_pFuncValues + I(x,y + 3), sizeof(glm::mat4x2) );

	return ret;
}

float SplineWarpedShadow::GetResolution()
{
	//-- potrebujeme o jedno vetsi rozliseni na kazde strane kvuli navic bodum pro spline
	return (IShadowTechnique::GetResolution() + 2.0);
}
