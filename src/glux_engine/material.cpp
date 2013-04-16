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

	 m_has_tessellation_shader = false;
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Load custom shader from file
@param source shader source file 
@return string with shader source
***************************************************************************************************/
string Material::LoadShader( const char* _filename )
{
    ifstream fin(_filename);
    if(!fin) 
    {
        string msg = "Cannot open shader ";
        msg += _filename;
        ShowMessage(msg.c_str(), false);
        return "null";
    }
    string data;
    char ch;
    while(fin.get(ch))
        data+=ch;

    return data;
}

//-----------------------------------------------------------------------------
