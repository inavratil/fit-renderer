/**
@file material.cpp
@brief material settings, dynamic shader creation
***************************************************************************************************/
#include "GeometryMaterial.h"

////////////////////////////////////////////////////////////////////////////////
//************************* GeometryMaterial methods ********************************//
////////////////////////////////////////////////////////////////////////////////


GeometryMaterial::GeometryMaterial( const char* _name ) :
	Material( _name, -1 )
{

	_Init(
		_name,
		-1,			//-- material id
		black, 		//-- ambient
		silver, 	//-- diffuse
		white,		//-- specular
        64.0, 		//-- shininess
		0.0, 		//-- reflection
		0.0, 		//-- transparency
		PHONG		//-- light model
		);
}
/**
****************************************************************************************************
@brief Direct creation of desired material
@param name material name (must be unique)
@param id material ID
@param amb ambient color (as RGB TVector)
@param diff diffuse color (as RGB TVector)
@param spec specular color (as RGB TVector)
@param shin shininess (0.0 - most shiny, 128.0 - least shiny)
@param reflect @todo material reflection
@param transp material transparency
@param lm light model (can be PHONG,GOURAUD,NONE)
***************************************************************************************************/
GeometryMaterial::GeometryMaterial(const char* name, int id, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm) :
	Material( name, id )
{
	_Init(
		name,
		id,
		amb,
		diff,
		spec,
		shin,
		reflect,
		transp,
		lm
		);
}

/**
****************************************************************************************************
@brief Destroy material data, detache shaders
***************************************************************************************************/
GeometryMaterial::~GeometryMaterial()
{
}

void GeometryMaterial::_Init( const char* name, int id, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm )
{
	m_ambColor = amb;
    m_diffColor = diff;
    m_specColor = spec;
    m_shininess = shin;
    m_reflection = reflect;
    m_transparency = transp;
    m_lightModel = lm;
    m_sceneID = 0;

    m_program = -1;
    m_baked = false;
    m_useMRT = false;
    m_custom_shader = false;
	m_is_screenspace = false;
    if(m_lightModel == SCREEN_SPACE)  //screen space quad cannot receive shadows
        m_receive_shadows = false;
    else
        m_receive_shadows = true;
}

#if 0
/**
****************************************************************************************************
@brief Add cubemap texture from external file. If texture has been loaded, a texture pointer from cache is used
instead of reloading from file
@param files external texture files with every side of cube texture (.tga)
@param textype texture type (CUBEMAP)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND,REPLACE)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param aniso should we use anisotropic filtering?
@param cache whether texture should be cached
@return new texture ID
*****************************************************************************************************/
GLint GeometryMaterial::AddTexture(const char **files, GLint textype, GLint texmode, GLfloat intensity, 
                            GLfloat tileX, GLfloat tileY, bool aniso, GLint cache)
{

    if(textype != CUBEMAP && textype != CUBEMAP_ENV)
    {
        cerr<<"ERROR: textype must be cubemap!\n";
        return -1;
    }

    ///1. generate new texture name (material name + texture mode (base, env, bump...) using GeometryMaterial::NextTexture()
    string texname = NextTexture(m_name + "CubeA");

    ///2. load new texture into map array (using Texture::Load())
    Texture *t = new Texture();
    m_textures[texname] = t;
    return m_textures[texname]->Load(texname.c_str(), textype, files, texmode, intensity, tileX, tileY, aniso, cache);
}

/**
****************************************************************************************************
@brief Add texture from external data.
@param texname texture name
@param textype texture type (can be BASE,ENV,BUMP,PARALLAX,DISPLACE,CUBEMAP,CUBEMAP_ENV, ALPHA, SHADOW, SHADOW_OMNI, 
                                    RENDER_TEXTURE, RENDER_TEXTURE_MULTISAMPLE)
@param texdata texture data in any format (int, float...)
@param tex_size texture width and height
@param tex_format OpenGL texture format (can be GL_RGBA, GL_RGBA16F or GL_RGBA32F)
@param tex_type internal data format (can be GL_UNSIGNED_BYTE or GL_FLOAT)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param mipmap sholud we generate mipmaps for texture?
@param aniso should we use anisotropic filtering?
@return new texture ID
****************************************************************************************************/
GLint GeometryMaterial::AddTextureData(const char *texname, GLint textype, const void *texdata, glm::vec2 tex_size, GLenum tex_format, GLenum data_type,
                                GLint texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso)
{
    //load new texture into map array (using Texture::Load())
    Texture *t = new Texture();
    m_textures[texname] = t;
    return m_textures[texname]->Load(texname, textype, texdata, tex_size, tex_format, data_type, texmode, intensity, tileX, tileY, mipmap, aniso);
}
#endif

/**
****************************************************************************************************
@brief Remove all shadow maps
***************************************************************************************************/
void GeometryMaterial::RemoveShadows()
{
    vector<string> to_erase;

    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
        if(strstr(m_it_textures->first.c_str(),"Shadow") != NULL)
            to_erase.push_back(m_it_textures->first);

    for(unsigned i=0; i<to_erase.size(); i++)
        m_textures.erase(to_erase[i]);

}


/**
****************************************************************************************************
@brief Render material. If hasn't been baked, bake him first(GeometryMaterial::BakeMaterial())
***************************************************************************************************/
int GeometryMaterial::RenderMaterial()
{
	int i = Material::RenderMaterial();

	for( m_if = m_features.begin(); m_if != m_features.end(); ++m_if )
	{
		(*m_if)->ActivateTextures( m_program, i );
	}

	return i;
}

//TODO: nekontroluje se, ze ficura jiz je v seznamu a pak se dvakrat priradi do shaderu stejny kus kodu
void GeometryMaterial::AddFeature( ShaderFeature* _pFeat )
{
	if( m_custom_shader || this->IsScreenSpace() )
		return;

	m_features.push_back( _pFeat );
}