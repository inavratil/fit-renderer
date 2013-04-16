#include "ScreenSpaceMaterial.h"

//-----------------------------------------------------------------------------

ScreenSpaceMaterial::ScreenSpaceMaterial( const char* _name, const char* _vs, const char* _fs, const char* _vs_defines, const char* _fs_defines ) :
	Material( _name )
{
	_Init( _name, _vs, _fs, _vs_defines, _fs_defines );
}

//-----------------------------------------------------------------------------

ScreenSpaceMaterial::~ScreenSpaceMaterial(void)
{
}

//-----------------------------------------------------------------------------

void ScreenSpaceMaterial::_Init( const char* _name, const char* _vs, const char* _fs, const char* _vs_defines, const char* _fs_defines )
{
	
	//load shader data
	m_v_source = _vs_defines + LoadShader( _vs );
	m_f_source = _fs_defines + LoadShader( _fs );

	m_is_screenspace = true;
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Loads and sets custom shader from source file. Fragment and vertex shader are mandatory, 
geometry and tesselation shaders optional
@param vertex vertex shader sources
@param tess_control tesselation control shader sources
@param tess_eval tessellation evaluation shader sources
@param geometry geometry shader sources
@param fragment fragment shader sources
***************************************************************************************************/
bool ScreenSpaceMaterial::BakeMaterial( int , int , bool  )
{
	if( m_baked ) return true;
	if( !BuildProgram() ) return false;
	if( !CheckShaderStatus() ) return false;

    //-- Get uniform variables for textures (using Texture::GetUniforms() )
    int i=0;
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        if(!m_it_textures->second->Empty())
        {
            m_it_textures->second->GetUniforms(m_program);
            m_it_textures->second->ActivateTexture(i,true);
            i++;
        }
    }
	
    m_baked = true;
    //m_custom_shader = true;
    return true;
}

//-----------------------------------------------------------------------------