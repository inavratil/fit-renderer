#include "ImprovedDPShadowMap.h"

//-----------------------------------------------------------------------------

ImprovedDPShadowMap::ImprovedDPShadowMap( TScene* _scene ) :
	DPShadowMap( _scene )
{
}

//-----------------------------------------------------------------------------

ImprovedDPShadowMap::~ImprovedDPShadowMap(void)
{
}

//-----------------------------------------------------------------------------

bool ImprovedDPShadowMap::Initialize()
{
	return true;
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::PreRender()
{
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::PostRender()
{
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::_PreDrawDepthMap()
{
}

//-----------------------------------------------------------------------------

glm::mat4 ImprovedDPShadowMap::_GetLightViewMatrix( int _i )
{
	return glm::mat4();
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::_UpdateShaderUniforms( int _i )
{
}

//-----------------------------------------------------------------------------

