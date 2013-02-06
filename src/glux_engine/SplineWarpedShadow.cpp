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

	Mcr = glm::mat4(
		 0.0f,	2.0f,	0.0f,	0.0f,
		-1.0f,	0.0f,	1.0f,	0.0f,
		 2.0f, -5.0f,	4.0f,  -1.0f,
		-1.0f,	3.0f,	-3.0f,  1.0f
		);
}

bool SplineWarpedShadow::Initialize(TLight* _light)
{
	return true;
}

void SplineWarpedShadow::PreRender()
{
	int res = (int)this->GetResolution();
	float* z_values = new float[res*res*2];
	
	memset(z_values, 0, res*res*2*sizeof(float));

	glBindTexture(GL_TEXTURE_2D, m_iTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, z_values);
	glBindTexture(GL_TEXTURE_2D, 0 ); 

	m_pFuncValues = z_values;

	this->GenerateGrid();

	delete[] z_values;
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
	
	glm::vec4 T, temp;
	glm::vec4 P0, P1, P2, P3;
	glm::vec4 Q;

	Splines4x4 splines;

	//-- prevod do intervalu [0..1], tj. pozice v bunce
	float x = glm::fract(_P.x);
	float y = glm::fract(_P.y);

	splines = _GetSplinesValues( grid_coords );

	//-- diff X
	T = glm::vec4( 1.0, x, glm::pow(x, 2.0f), glm::pow(x,3.0f) );

	P0 = glm::vec4( splines.p0[0].x,	splines.p0[1].x,	splines.p0[2].x,	splines.p0[3].x );
	P1 = glm::vec4( splines.p1[0].x,	splines.p1[1].x,	splines.p1[2].x,	splines.p1[3].x );
	P2 = glm::vec4( splines.p2[0].x,	splines.p2[1].x,	splines.p2[2].x,	splines.p2[3].x );
	P3 = glm::vec4( splines.p3[0].x,	splines.p3[1].x,	splines.p3[2].x,	splines.p3[3].x );

	temp = P0 * 0.5 * Mcr;
	Q.x = glm::dot(temp,T);
	temp = P1 * 0.5 * Mcr;
	Q.y = glm::dot(temp,T);
	temp = P2 * 0.5 * Mcr;
	Q.z = glm::dot(temp,T);
	temp = P3 * 0.5 * Mcr;
	Q.w = glm::dot(temp,T);

	T = glm::vec4( 1.0, y, glm::pow(y, 2.0f), glm::pow(y, 3.0f) );

	temp = Q * 0.5 * Mcr;
	diff.x = glm::dot( temp, T );

	//-- diff Y
	T = glm::vec4( 1.0, x, glm::pow(x, 2.0f), glm::pow(x,3.0f) );

	P0 = glm::vec4( splines.p0[0].y,	splines.p0[1].y,	splines.p0[2].y,	splines.p0[3].y );
	P1 = glm::vec4( splines.p1[0].y,	splines.p1[1].y,	splines.p1[2].y,	splines.p1[3].y );
	P2 = glm::vec4( splines.p2[0].y,	splines.p2[1].y,	splines.p2[2].y,	splines.p2[3].y );
	P3 = glm::vec4( splines.p3[0].y,	splines.p3[1].y,	splines.p3[2].y,	splines.p3[3].y );

	temp = P0 * 0.5 * Mcr;
	Q.x = glm::dot(temp,T);
	temp = P1 * 0.5 * Mcr;
	Q.y = glm::dot(temp,T);
	temp = P2 * 0.5 * Mcr;
	Q.z = glm::dot(temp,T);
	temp = P3 * 0.5 * Mcr;
	Q.w = glm::dot(temp,T);

	T = glm::vec4( 1.0, y, glm::pow(y, 2.0f), glm::pow(y, 3.0f) );

	temp = Q * 0.5 * Mcr;
	diff.y = glm::dot( temp, T );

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
