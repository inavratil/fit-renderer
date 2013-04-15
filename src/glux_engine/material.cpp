#include "Material.h"

//-----------------------------------------------------------------------------

Material::Material( const char* _name )
{
	_Init( _name, -1 );
}

//-----------------------------------------------------------------------------

Material::Material( const char* _name, int _id )
{
	_Init( _name, _id );
}

//-----------------------------------------------------------------------------

Material::~Material(void)
{
	//-- free textures
	//for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); m_it_textures++)
	//	if( m_it_textures->second )
	//		delete m_it_textures->second;
}

//-----------------------------------------------------------------------------

void Material::_Init( const char* _name, int _id )
{
	m_name = _name;
	m_id = _id;
}

//-----------------------------------------------------------------------------
