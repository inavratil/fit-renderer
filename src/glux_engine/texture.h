/**
****************************************************************************************************
****************************************************************************************************
@file: texture.h
@brief load TGA textures from file and converts them to suitable OpenGL format. TGA files can be
* compressed or uncompressed
****************************************************************************************************
***************************************************************************************************/
#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "globals.h"

//-- Texture targets
const GLenum TEX_2D			= GL_TEXTURE_2D;
const GLenum TEX_3D			= GL_TEXTURE_3D;
const GLenum TEX_2D_ARRAY	= GL_TEXTURE_2D_ARRAY;
const GLenum TEX_CUBE		= GL_TEXTURE_CUBE_MAP;

//-- Parameters
const GLenum TEX_WRAP_S		= GL_TEXTURE_WRAP_S;
const GLenum TEX_WRAP_T		= GL_TEXTURE_WRAP_T;

///@class Texture 
///@brief holds texture parameters and contains functions to load texture from external file
///Textures are connected to shaders via uniform variables
class Texture
{

//-----------------------------------------------------------------------------
//-- Member variables

private:
	GLuint	m_texID;                 //texture ID
	GLuint	m_last_tex;				//-- lastly bound texture
    int		m_textype;                  //texture type
    string	m_texname;               //texture name
	GLuint	m_target;				//-- texture target

    GLubyte *m_imageData;           //image data
    GLuint m_width, m_height, m_bpp;    //width, height
    
    int m_texmode;

    GLfloat m_intensity;            //texture intensity
    GLfloat m_tileX, m_tileY;       //texture tiles
    //shader uniform variables
    GLint m_texLoc, m_tileXLoc, m_tileYLoc, m_intensityLoc;

	static bool isILInitialized;

//-----------------------------------------------------------------------------
//-- Public methods 
public:
    Texture( GLenum _target = TEX_2D );
    ~Texture();

	void Init( GLenum _target );
	void Destroy();

//-----------------------------------------------------------------------------
	GLuint Bind();
	void Unbind();

	void SetFiltering( GLenum _filter );
	void SetWrap( GLenum _param );

	//-- Set/Get target
	void SetTarget( GLenum _target ){ m_target = _target; }
	GLenum GetTarget(){	return m_target; }

	//-- Set/Get
	void SetWidth( GLuint _width ){ m_width = _width; }
	void SetHeight( GLuint _height ){ m_height = _height; }
	void SetSize( GLuint _width, GLuint _height ){ m_width = _width; m_height = _height; }
	void SetBpp( GLuint _bpp ){ m_bpp = _bpp; }

    ///@brief set texture name
    void SetName(string name){ m_texname = name; }
    ///@brief get texture name
    string GetName(){ return m_texname; }

    //create texture from TGA file
    GLint Load(const char *texname, int textype, const char *filename, int texmode, 
               GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso, GLint cache);

    //create cubemap fro six TGA files
    GLint Load(const char *texname, int textype, const char **cube_files,
               int texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool aniso, GLint cache);

    //create texture directly from data
    GLint Load(const char *texname, int textype, const void *texdata, glm::vec2 tex_size, GLenum tex_format, GLenum data_type,
               int texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso);

    //load TGA texture from file
    bool LoadImage(const char *filename);

    ///@brief do we have image data?
    bool Empty(){ 
        return (m_texID == 0); 
    } 

    //activate texture for use by shader
    void ActivateTexture(GLint tex_unit, bool set_uniforms = false);
    //get uniform variables for texture
    void GetUniforms(GLuint shader);
 
    ///@brief set texture intensity
    void SetIntensity(GLfloat _intensity){ 
        m_intensity = _intensity; 
    }
    float GetIntensity(){
        return m_intensity;
    }
    ///@brief Set texture ID
    void SetID(GLuint id){ 
        m_texID = id; 
    }
	///@brief Get OpenGL texture ID
    GLuint GetID(){ 
        return m_texID; 
    }
    ///@brief Set texture type
    void SetType(int type){ 
        m_textype = type; 
    }
    ///@brief Get texture type
    int GetType(){ 
        return m_textype; 
    }
    ///@brief Get texture mode
    int GetMode(){ 
        return m_texmode; 
    }
    ///@brief Has texture tiles?
    bool HasTiles(){ 
        return (m_tileX > 1 || m_tileY > 1); 
    }
};

typedef Texture* TexturePtr;

#endif
