#include "PolynomialWarpedShadow.h"

#include "shadow.h"

PolynomialWarpedShadow::PolynomialWarpedShadow()
	: m_matCoeffsX(1.0), m_matCoeffsY(1.0)
{
}

PolynomialWarpedShadow::PolynomialWarpedShadow( glm::mat4 _coeffsX, glm::mat4 _coeffsY )
	: m_matCoeffsX(_coeffsX), m_matCoeffsY(_coeffsY)
{
}


PolynomialWarpedShadow::~PolynomialWarpedShadow(void)
{
}

bool PolynomialWarpedShadow::Initialize(TLight* _light)
{
	return true;
}

void PolynomialWarpedShadow::PreRender()
{
}

void PolynomialWarpedShadow::PostRender()
{
}

glm::vec2 PolynomialWarpedShadow::ComputeDiff( glm::vec2 _P )
{
	glm::vec2 diff;
	glm::vec4 temp, X, Y;
	
	X = glm::vec4( 1.0, _P.x, glm::pow(_P.x, 2.0f), glm::pow(_P.x,3.0f) );
	Y = glm::vec4( 1.0, _P.y, glm::pow(_P.y, 2.0f), glm::pow(_P.y,3.0f) );

	//FIXME: nemusi ty matice byt transponovane?? V octavu jsem je musel transponovat.
	temp = X * m_matCoeffsX;
	diff.x = glm::dot(temp, Y) * POLY_BIAS;
	temp = X * m_matCoeffsY;
	diff.y = glm::dot(temp, Y) * POLY_BIAS;

	return diff;
}


float PolynomialWarpedShadow::GetResolution()
{
	return 4.0;
}

///////////////////////////////////////////////////////////////////////////////

void PolynomialWarpedShadow::SetMatrices( glm::mat4 _coeffsX, glm::mat4 _coeffsY )
{
	m_matCoeffsX = _coeffsX;
	m_matCoeffsY = _coeffsY;
}

void PolynomialWarpedShadow::SetMatCoeffsX( glm::mat4 _coeffsX )
{
	m_matCoeffsX = _coeffsX;
}

void PolynomialWarpedShadow::SetMatCoeffsY( glm::mat4 _coeffsY )
{
	m_matCoeffsY = _coeffsY;
}

glm::mat4 PolynomialWarpedShadow::GetMatCoeffsX()
{
	return m_matCoeffsX;
}

glm::mat4 PolynomialWarpedShadow::GetMatCoeffsY()
{
	return m_matCoeffsY;
}
