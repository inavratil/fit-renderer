#include "BilinearWarpedShadow.h"

#include "shadow.h"

BilinearWarpedShadow::BilinearWarpedShadow()
	: m_fRes(16.0)
{
}


BilinearWarpedShadow::~BilinearWarpedShadow(void)
{
}

bool BilinearWarpedShadow::Initialize(TLight* _light)
{
	return true;
}

void BilinearWarpedShadow::PreRender()
{
}

void BilinearWarpedShadow::PostRender()
{
}

glm::vec2 BilinearWarpedShadow::ComputeDiff( glm::vec2 _P )
{
	glm::vec2 diff( 0.0 );

	return diff;
}

float BilinearWarpedShadow::GetResolution()
{
	return m_fRes;
}
