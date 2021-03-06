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

///Two possible types of TGA image
enum TGAtypes{COMPRESSED,UNCOMPRESSED};

///@class Texture 
///@brief holds texture parameters and contains functions to load texture from external file
///Textures are connected to shaders via uniform variables
class Texture
{
private:
    int m_textype;                  //texture type
    string m_texname;               //texture name

    GLubyte *m_imageData;           //image data
    GLuint m_width, m_height, m_bpp;    //width, height
    GLuint m_texID;                 //texture ID
    int m_texmode;

    GLfloat m_intensity;            //texture intensity
    GLfloat m_tileX, m_tileY;       //texture tiles
    //shader uniform variables
    GLint m_texLoc, m_tileXLoc, m_tileYLoc, m_intensityLoc;

	static bool isILInitialized;

public:    
    Texture();
    ~Texture();

    ///@brief set texture name
    void SetName(string name){ 
        m_texname = name; 
    }
    ///@brief get texture name
    string GetName(){ 
        return m_texname; 
    }

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
    ///@brief Set texture type
    void SetType(int type){ 
        m_textype = type; 
    }

    ///@brief Get OpenGL texture ID
    GLuint GetID(){ 
        return m_texID; 
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

#endif
