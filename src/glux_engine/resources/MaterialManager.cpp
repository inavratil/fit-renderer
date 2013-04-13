#include "MaterialManager.h"

//-----------------------------------------------------------------------------

MaterialManager::MaterialManager(void)
{
}

//-----------------------------------------------------------------------------

MaterialManager::~MaterialManager(void)
{
}

//-----------------------------------------------------------------------------

MaterialPtr MaterialManager::Create( const char* _name )
{
	MaterialPtr mat = new Material( _name );

	return NULL;
}
//-----------------------------------------------------------------------------
