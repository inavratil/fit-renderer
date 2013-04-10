#include "Shader.h"

//-----------------------------------------------------------------------------

Shader::Shader( const char* _name, unsigned _id ) :
	m_name( _name ),
	m_id( _id )
{
}

//-----------------------------------------------------------------------------

Shader::~Shader(void)
{
	//-- free textures
	for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); m_it_textures++)
		delete m_it_textures->second;
}

//-----------------------------------------------------------------------------
