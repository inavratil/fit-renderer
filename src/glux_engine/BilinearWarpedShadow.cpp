#include "BilinearWarpedShadow.h"

#include "shadow.h"

BilinearWarpedShadow::BilinearWarpedShadow()
{
	_Init();
}


BilinearWarpedShadow::~BilinearWarpedShadow(void)
{
}

void BilinearWarpedShadow::_Init()
{
	m_sName = "Bilinear";
}

bool BilinearWarpedShadow::Initialize(TLight* _light)
{
	return true;
}

void BilinearWarpedShadow::PreRender()
{
	this->GenerateGrid();
}

void BilinearWarpedShadow::PostRender()
{
}

glm::vec2 BilinearWarpedShadow::ComputeDiff( glm::vec2 _P )
{
	glm::vec2 diff( 0.0 );

	return diff;
}
