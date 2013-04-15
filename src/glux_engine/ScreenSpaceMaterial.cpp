#include "ScreenSpaceMaterial.h"

//-----------------------------------------------------------------------------

ScreenSpaceMaterial::ScreenSpaceMaterial( const char* _name, const char* _vs, const char* _fs ) :
	Material( _name )
{
}

//-----------------------------------------------------------------------------

ScreenSpaceMaterial::~ScreenSpaceMaterial(void)
{
}

//-----------------------------------------------------------------------------

void ScreenSpaceMaterial::_Init()
{
	m_is_screenspace = true;
}

//-----------------------------------------------------------------------------