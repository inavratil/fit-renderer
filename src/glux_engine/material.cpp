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

	m_has_alpha_channel = false;
	m_has_tessellation_shader = false;
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief Finds and returns next free texture name in list (important to synchronize generated texture names
to them stored in TMaterial::textures field)
@param texname suffix, texture type (like Base, Bump, Env...) 
@return new texture name
***************************************************************************************************/
string Material::NextTexture( string _texname )
{
    int i = 1;
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        ///find first empty texture, add index to m_it_textures's name and return this name
        if(_texname == m_it_textures->first || m_it_textures->first.empty() )
        {
            string rep;
            rep = num2str(char('A' + i));
            _texname.replace(_texname.length()-1, rep.size(), rep.c_str());
            i++;
        }
    }
    return _texname;
}

//-----------------------------------------------------------------------------

GLuint Material::AddTexturePtr( TexturePtr _tex )
{
	//-- 1. generate new texture name (material name + texture mode (base, env, bump...) using TMaterial::NextTexture()
    string texname;
	switch(_tex->GetType())
    {
        //base map
    case BASE: texname = NextTexture(m_name + "_Base_A"); break;
        //alpha map
    case ALPHA: texname = NextTexture(m_name + "_Alpha_A"); m_has_alpha_channel = true; break;
        //environment map
    case ENV: texname = NextTexture(m_name + "_Env_A"); break;
        //bump map
    case BUMP: texname = NextTexture(m_name + "_Bump_A"); break;
        //parallax map
    case PARALLAX: texname = NextTexture(m_name + "_Parallax_A"); break;   
        //displacement map
    case DISPLACE: texname = NextTexture(m_name + "_Displace_A"); break; 
        //cube map
    case CUBEMAP: texname = NextTexture(m_name + "_Cube_A"); break; 
        //environment cube map
    case CUBEMAP_ENV: texname = m_name + "_CubeEnv_A"; break;//NextTexture(m_name + "CubeEnvA"); break; 
        //render textures
    case RENDER_TEXTURE: 
    case RENDER_TEXTURE_MULTISAMPLE: 
        texname = _tex->GetName(); 
        break; 
	}

	m_textures[texname] = _tex;
	return _tex->GetID();
}

//-----------------------------------------------------------------------------

void Material::RemoveTexture( const char *_texName )
{	
	m_textures.erase(_texName);
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

int Material::RenderMaterial()
{
	#ifdef VERBOSE
    cout<<"Rendering "<<m_name;
	#endif
    ///enable shader
    glUseProgram(m_program);

    ///activate textures attached to material (Texture::ActivateTexture() )
    int i=0;
    for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures)
    {
        if(!m_it_textures->second->Empty())
        {
            m_it_textures->second->ActivateTexture(i);
            i++;
        }
    }

	return i;
}

//-----------------------------------------------------------------------------