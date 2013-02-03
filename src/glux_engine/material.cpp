/**
@file material.cpp
@brief material settings, dynamic shader creation
***************************************************************************************************/
#include "material.h"

/**
****************************************************************************************************
@brief Load custom shader from file
@param source shader source file 
@return string with shader source
***************************************************************************************************/
string LoadShader(const char* source)
{
    ifstream fin(source);
    if(!fin) 
    {
        string msg = "Cannot open shader ";
        msg += source;
        ShowMessage(msg.c_str(), false);
        return "null";
    }
    string data;
    char ch;
    while(fin.get(ch))
        data+=ch;

    return data;
}

////////////////////////////////////////////////////////////////////////////////
//************************* TMaterial methods ********************************//
////////////////////////////////////////////////////////////////////////////////


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
TMaterial::TMaterial(const char* name, unsigned id, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm)
{
    m_name = name;
    m_matID = id;
    m_ambColor = amb;
    m_diffColor = diff;
    m_specColor = spec;
    m_shininess = shin;
    m_reflection = reflect;
    m_transparency = transp;
    m_lightModel = lm;
    m_sceneID = 0;

    m_shader = -1;
    m_baked = false;
    m_useMRT = false;
    m_custom_shader = false;
    if(m_lightModel == SCREEN_SPACE)  //screen space quad cannot receive shadows
        m_receive_shadows = false;
    else
        m_receive_shadows = true;
    m_is_alpha = false;
    m_is_tessellated = false;
}

/**
****************************************************************************************************
@brief Destroy material data, detache shaders
***************************************************************************************************/
TMaterial::~TMaterial()
{
    if(m_shader != 0)
    {
        glDetachObjectARB(m_shader,m_f_shader);
        glDetachObjectARB(m_shader,m_g_shader);
        glDetachObjectARB(m_shader,m_v_shader);
        glDeleteObjectARB(m_shader);
    }
    //free textures
    for(m_it = m_textures.begin(); m_it != m_textures.end(); m_it++)
        delete m_it->second;
}


/**
****************************************************************************************************
@brief Finds and returns next free texture name in list (important to synchronize generated texture names
to them stored in TMaterial::textures field)
@param texname suffix, texture type (like Base, Bump, Env...) 
@return new texture name
***************************************************************************************************/
string TMaterial::NextTexture(string texname)
{
    int i = 1;
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        ///find first empty texture, add index to m_it's name and return this name
        if(texname == m_it->first || m_it->first.empty() )
        {
            string rep;
            rep = num2str(char('A' + i));
            texname.replace(texname.length()-1, rep.size(), rep.c_str());
            i++;
        }
    }
    return texname;
}

GLint TMaterial::LoadTexture(string _texname, int _textype, const char *filename, int _texmode,
                    GLfloat _intensity, GLfloat _tileX, GLfloat _tileY, bool mipmap, bool aniso, GLint cache)
{
    string name = NextTexture( _texname + "AuxA" );
    return m_textures[name]->Load(name.c_str(), _textype, filename, _texmode, _intensity, _tileX, _tileY, mipmap, aniso, cache);
}


/**
****************************************************************************************************
@brief Add texture from external file. If texture has been loaded, a texture pointer from cache is used
instead of reloading from file
@param file external texture file (.tga)
@param textype texture type (can be BASE,ENV,BUMP,PARALLAX,DISPLACE,CUBEMAP,CUBEMAP_ENV, ALPHA, SHADOW, SHADOW_OMNI, 
                                    RENDER_TEXTURE, RENDER_TEXTURE_MULTISAMPLE)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND,REPLACE)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param mipmap should we generate mipmaps for texture?
@param aniso should we use anisotropic filtering?
@param cache whether texture should be cached
@return new texture ID
****************************************************************************************************/
GLint TMaterial::AddTexture(const char *file, GLint textype, GLint texmode, GLfloat intensity, 
                            GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso, GLint cache)
{
    ///1. generate new texture name (material name + texture mode (base, env, bump...) using TMaterial::NextTexture()
    string texname;
    switch(textype)
    {
        //base map
    case BASE: texname = NextTexture(m_name + "BaseA"); break;
        //alpha map
    case ALPHA: texname = NextTexture(m_name + "AlphaA"); m_is_alpha = true; break;
        //environment map
    case ENV: texname = NextTexture(m_name + "EnvA"); break;
        //bump map
    case BUMP: texname = NextTexture(m_name + "BumpA"); break;
        //parallax map
    case PARALLAX: texname = NextTexture(m_name + "ParallaxA"); break;   
        //displacement map
    case DISPLACE: texname = NextTexture(m_name + "DisplaceA"); break; 
        //cube map
    case CUBEMAP: texname = NextTexture(m_name + "CubeA"); break; 
        //environment cube map
    case CUBEMAP_ENV: texname = m_name + "CubeEnvA"; break;//NextTexture(m_name + "CubeEnvA"); break; 
        //render textures
    case RENDER_TEXTURE: 
    case RENDER_TEXTURE_MULTISAMPLE: 
        texname = file; 
        break; 

    default: texname = NextTexture(m_name + "BaseA"); break;
    };

    ///2. load new texture into map array (using Texture::Load())
    Texture *t = new Texture();
    m_textures[texname] = t;
    return m_textures[texname]->Load(texname.c_str(), textype, file, texmode, intensity, tileX, tileY, mipmap, aniso, cache);
}

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
GLint TMaterial::AddTexture(const char **files, GLint textype, GLint texmode, GLfloat intensity, 
                            GLfloat tileX, GLfloat tileY, bool aniso, GLint cache)
{

    if(textype != CUBEMAP && textype != CUBEMAP_ENV)
    {
        cerr<<"ERROR: textype must be cubemap!\n";
        return -1;
    }

    ///1. generate new texture name (material name + texture mode (base, env, bump...) using TMaterial::NextTexture()
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
GLint TMaterial::AddTextureData(const char *texname, GLint textype, const void *texdata, glm::vec2 tex_size, GLenum tex_format, GLenum data_type,
                                GLint texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso)
{
    //load new texture into map array (using Texture::Load())
    Texture *t = new Texture();
    m_textures[texname] = t;
    return m_textures[texname]->Load(texname, textype, texdata, tex_size, tex_format, data_type, texmode, intensity, tileX, tileY, mipmap, aniso);
}

/**
****************************************************************************************************
@brief Add shadow map
@param type shadow type (spot or omni)
@param map pointer to texture data
@param intensity shadow intensity (0 - transparent, 1 - opaque)
***************************************************************************************************/
void TMaterial::AddTextureFromCache(int type, GLuint id, GLfloat intensity)
{
	//FIXME: need to check if texutre with id exists in the cache

    //don't add shadow map for materials whose don't want to receive shadows!
    if(!m_receive_shadows) 
        return;

	//create texture
    Texture *tex = new Texture();
    tex->SetID(id);

    ///1. generate new shadow texture name using TMaterial::NextTexture()
    string texname;
	switch(type)
	{
	case SPOT: 
		texname = NextTexture(m_name + "ShadowA"); 
		tex->SetType(SHADOW);
		break;
	case OMNI: 
		texname = NextTexture(m_name + "ShadowOMNI_A"); 
		tex->SetType(SHADOW_OMNI);
		break;
	case CUSTOM:
		texname = NextTexture(m_name + "Custom_A"); 
		tex->SetType(CUSTOM);
		break;
	}
	
    tex->SetName(texname);      

    ///@todo 2: if material is transparent, modify intensity of shadow
    if(m_transparency > 0.0) 
        intensity = m_transparency;

    tex->SetIntensity(intensity);
    //add into list
    m_textures[texname] = tex;
}


/**
****************************************************************************************************
@brief Remove all shadow maps
***************************************************************************************************/
void TMaterial::RemoveShadows()
{
    vector<string> to_erase;

    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
        if(strstr(m_it->first.c_str(),"Shadow") != NULL)
            to_erase.push_back(m_it->first);

    for(unsigned i=0; i<to_erase.size(); i++)
        m_textures.erase(to_erase[i]);

}


/**
****************************************************************************************************
@brief Render material. If hasn't been baked, bake him first(TMaterial::BakeMaterial())
***************************************************************************************************/
void TMaterial::RenderMaterial()
{
#ifdef VERBOSE
    cout<<"Rendering "<<m_name;
#endif
    ///enable shader
    glUseProgram(m_shader);

    ///activate textures attached to material (Texture::ActivateTexture() )
    int i=0;
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        if(!m_it->second->Empty())
        {
            m_it->second->ActivateTexture(i);
            i++;
        }
    }
#ifdef VERBOSE
    cout<<"...done\n";
#endif
}



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
bool TMaterial::CustomShader(TShader *vertex, TShader *tess_control, TShader *tess_eval, TShader *geometry, TShader *fragment)
{
    //check OpenGL 4 support(when tessellation shaders present)
    m_is_tessellated = false;
    if(tess_control != NULL && tess_eval != NULL && !GLEW_ARB_gpu_shader5)
    {
        cout<<"OpenGL 4 not supported, can't use tessellation!\n";
        return false;
    }

    //create shaders for vertex and fragment shader
    m_v_shader = glCreateShader(GL_VERTEX_SHADER);
    m_f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //shader version. If OpenGL4 is supported, use 400, else 330
    string version = "#version 330 compatibility\n";
    if(GLEW_ARB_gpu_shader5)
        version = "#version 400 core\n";

    //write shader header - version and defines
    string vertex_shader, fragment_shader, geometry_shader, tess_control_shader, tess_eval_shader;
    vertex_shader = version + vertex->defines;
    fragment_shader = version + fragment->defines;

    //load shader data
    vertex_shader += LoadShader(vertex->source.c_str());
    fragment_shader += LoadShader(fragment->source.c_str());

    if(vertex_shader == "null" || fragment_shader == "null")
        return false;

    const char *ff = fragment_shader.c_str();
    const char *vv = vertex_shader.c_str();

    //set shader source
    glShaderSource(m_v_shader, 1, &vv,NULL);
    glShaderSource(m_f_shader, 1, &ff,NULL);

    //compile and detect shader errors
    char log[BUFFER]; int len;
    glCompileShader(m_v_shader);
    glCompileShader(m_f_shader);

    //create and link shader program
    m_shader = glCreateProgram();
    glAttachShader(m_shader,m_f_shader);
    glAttachShader(m_shader,m_v_shader);


    //do we have tessellation shaders?
    if(tess_control != NULL && tess_eval != NULL)
    {
        m_is_tessellated = true;
        //then load, create, compile and attach tessellation shaders
        m_tc_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
        m_te_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        string tess_control_shader = version + tess_control->defines;
        string tess_eval_shader = version + tess_eval->defines;

        //load and link shader
        tess_control_shader += LoadShader(tess_control->source.c_str());
        tess_eval_shader += LoadShader(tess_eval->source.c_str());
        if(tess_control_shader == "null" || tess_eval_shader == "null")
            return false;
        const char *tc = tess_control_shader.c_str();
        const char *te = tess_eval_shader.c_str();
        glShaderSource(m_tc_shader, 1, &tc, NULL);
        glShaderSource(m_te_shader, 1, &te, NULL);
        glCompileShader(m_tc_shader);
        glCompileShader(m_te_shader);
        glAttachShader(m_shader,m_tc_shader);
        glAttachShader(m_shader,m_te_shader);
    }

    //do we have geometry shader?
    if(geometry != NULL)
    {
        //then load, create, compile and attach geometry shader
        m_g_shader = glCreateShader(GL_GEOMETRY_SHADER);
        string geometry_shader = version + geometry->defines;

        //load and link shader
        geometry_shader += LoadShader(geometry->source.c_str());
        if(geometry_shader == "null")
            return false;
        const char *gg = geometry_shader.c_str();
        glShaderSource(m_g_shader, 1, &gg,NULL);
        glCompileShader(m_g_shader);
        glAttachShader(m_shader,m_g_shader);
    }

    //final shader linking
    glLinkProgram(m_shader);
    glUseProgram(m_shader);

    //shader creation status: print error if any
    ofstream fout("shader_log.txt", ios_base::app);
    glGetShaderInfoLog(m_v_shader, BUFFER, &len, log);
    bool compile_err = false;
    //log from vertex shader
    if(strstr(log, "succes") == NULL && len > 0) 
    {
        fout<<endl<<m_name<<":"<<log;  
        cout<<endl<<m_name<<":"<<log;  
        compile_err = true;
    }
    //log from tessellation shaders
    if(tess_control != NULL && tess_eval != NULL)
    {
        glGetShaderInfoLog(m_tc_shader, BUFFER, &len, log);
        if(strstr(log, "succes") == NULL && len > 0) 
        {
            fout<<endl<<m_name<<":"<<log;  
            cout<<endl<<m_name<<":"<<log;
            compile_err = true;
        }
        glGetShaderInfoLog(m_te_shader, BUFFER, &len, log);
        if(strstr(log, "succes") == NULL && len > 0) 
        {
            fout<<endl<<m_name<<":"<<log;  
            cout<<endl<<m_name<<":"<<log;
            compile_err = true;
        }
    }
    //log from geometry shader
    if(geometry != NULL)
    {
        glGetShaderInfoLog(m_g_shader, BUFFER, &len, log);
        if(strstr(log, "succes") == NULL && len > 0) 
        {
            fout<<endl<<m_name<<":"<<log;  
            cout<<endl<<m_name<<":"<<log;
            compile_err = true;
        }
    }
    //log from fragment shader
    glGetShaderInfoLog(m_f_shader, BUFFER, &len, log);
    if(strstr(log, "succes") == NULL && len > 0) 
    {
        fout<<endl<<m_name<<":"<<log;   
        cout<<endl<<m_name<<":"<<log;
        compile_err = true;
    }
    fout.close();



    //*************************************
    ///4 Get uniform variables for textures (using Texture::GetUniforms() )
    int i=0;
    for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it)
    {
        if(!m_it->second->Empty())
        {
            m_it->second->GetUniforms(m_shader);
            m_it->second->ActivateTexture(i,true);
            i++;
        }
    }

    ///set materials parameters
    if(!IsScreenSpace())
    {
        SetUniform("material.ambient",m_ambColor);
        SetUniform("material.diffuse",m_diffColor);
        SetUniform("material.specular",m_specColor);
        SetUniform("material.shininess",m_shininess);

        //setup uniform buffers
        GLint uniformIndex = glGetUniformBlockIndex(m_shader, "Matrices");
        if(uniformIndex >= 0)
            glUniformBlockBinding(m_shader, uniformIndex, UNIFORM_MATRICES);
        uniformIndex = glGetUniformBlockIndex(m_shader, "Lights");
        if(uniformIndex >= 0)
            glUniformBlockBinding(m_shader, uniformIndex, UNIFORM_LIGHTS);
    }

    glUseProgram(0);
    m_baked = true;
    m_custom_shader = true;
    return !compile_err;
}

void TMaterial::AddFeature( ShaderFeature* _pFeat )
{
	if( m_custom_shader || this->IsScreenSpace() )
		return;

	m_features.push_back( _pFeat );
}