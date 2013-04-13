/**
****************************************************************************************************
****************************************************************************************************
@file: texture.cpp
@brief 
****************************************************************************************************
***************************************************************************************************/
#include "texture.h"

bool Texture::isILInitialized = false;

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief reset basic values (null name and texture data)
****************************************************************************************************/
Texture::Texture()
{
	Init();
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief free all texture data by detaching texture
@todo texture destruction might be called after GL context is destroyed, so it has no effect
****************************************************************************************************/
Texture::~Texture()
{
    Destroy();
}

//-----------------------------------------------------------------------------

void Texture::Init()
{
	//-- init properties
	m_texname = "null";
    m_textype = BASE;
    m_imageData = NULL;
    m_last_tex = m_width = m_height = m_bpp = 0;
    m_texmode = MODULATE;
    m_tileX = m_tileY = 1.0;
    m_texLoc = m_tileXLoc = m_tileYLoc = -1;
	m_target = GL_TEXTURE_2D;

	//-- init texture id
	glGenTextures( 1, &m_texID );
}

//-----------------------------------------------------------------------------

void Texture::Destroy()
{
	glDeleteTextures( 1, &m_texID );
	m_texID = 0;
}

//-----------------------------------------------------------------------------

GLuint Texture::Bind()
{
	//-- save currently bound FBO
	int tmp;
	switch( m_target )
	{
	case TEX_2D: 
		glGetIntegerv( GL_TEXTURE_BINDING_2D, &tmp );
		break;
	case TEX_3D: 
		glGetIntegerv( GL_TEXTURE_BINDING_3D, &tmp );
		break;
	case TEX_2D_ARRAY: 
		glGetIntegerv( GL_TEXTURE_BINDING_2D_ARRAY, &tmp );
		break;
	case TEX_CUBE: 
		glGetIntegerv( GL_TEXTURE_BINDING_CUBE_MAP, &tmp );
		break;
	}

	m_last_tex = tmp;

	//-- we don't need to bind the same texture
	if( m_last_tex != m_texID )
		glBindTexture( m_target, m_texID );	

	return m_last_tex;
}

//-----------------------------------------------------------------------------

void Texture::Unbind()
{
	//-- check currently bound texture
	int tmp;
	switch( m_target )
	{
	case TEX_2D: 
		glGetIntegerv( GL_TEXTURE_BINDING_2D, &tmp );
		break;
	case TEX_3D: 
		glGetIntegerv( GL_TEXTURE_BINDING_3D, &tmp );
		break;
	case TEX_2D_ARRAY: 
		glGetIntegerv( GL_TEXTURE_BINDING_2D_ARRAY, &tmp );
		break;
	case TEX_CUBE: 
		glGetIntegerv( GL_TEXTURE_BINDING_CUBE_MAP, &tmp );
		break;
	}
	//-- we don't have to unbind different texture
	if( tmp != m_texID ) return;

	glBindTexture( m_target, m_last_tex );
	m_last_tex = 0;
}

//-----------------------------------------------------------------------------

void Texture::SetFiltering( GLenum _filter, bool _mipmaps )
{
	Bind();
	Unbind();
}

//-----------------------------------------------------------------------------

/**
****************************************************************************************************
@brief load TGA texture as texture image. If texture has been loaded, a texture pointer from cache is used
instead of reloading from file
@param texname texture name
@param textype texture type (can be BASE,ENV,BUMP)
@param filename external texture file (.tga)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param mipmap sholud we generate mipmaps for texture?
@param aniso should we use anisotropic filtering?
@param cache whether we should use cached texture image or load new
@return new texture ID
****************************************************************************************************/
GLint Texture::Load(const char *texname, int textype, const char *filename, int texmode,
    GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso, GLint cache)
{
    ///if texture is loaded in cache, copy only pointer to texture data
    if(cache != -1)
    {
        m_texID = cache;
    }
    else      //load texture from file
    {
        if(!LoadImage(filename))
            return ERR;

        //texture generation
        glBindTexture(GL_TEXTURE_2D, m_texID);

        //texture with anisotropic filtering
        if(aniso)
        {
            //find out, if GFX supports aniso filtering
            if(!GLEW_EXT_texture_filter_anisotropic)
            {
                cout<<"Anisotropic filtering not supported. Using linear instead.\n";
            }
            else
            {
                float maxAnisotropy;
                //find out maximum supported anisotropy
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
            }
        }
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //mip-mapped texture (if set)
        if(mipmap)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        if(textype == BUMP)	//don't compress bump maps
#ifdef _LINUX_
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_imageData);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData);
#endif
        else					//compress color maps
#ifdef _LINUX_
            glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_imageData);
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData);
#endif
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if(mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);

        delete [] m_imageData;      //don't need image data after texture was created
    }

    //texture mode and type
    m_texname = texname;
    m_textype = textype;
    m_texmode = texmode;
    m_tileX = tileX;
    m_tileY = tileY;
    m_intensity = intensity;

    //cout<<"Texture \""<<texname<<"\" creation OK\n";

    return m_texID;
}


/**
****************************************************************************************************
@brief load 6 TGA texture as cube map. If texture has been loaded, a texture pointer from cache is used
instead of reloading from file
@param texname texture name
@param textype texture type (can be BASE,ENV,BUMP)
@param cube_files array of strings with filenames containing every side of cube map (.tga)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param aniso should we use anisotropic filtering?
@param cache whether we should use cached texture image or load new
@return new texture ID
****************************************************************************************************/

GLint Texture::Load(const char *texname, int textype,
    const char **cube_files, int texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool aniso, GLint cache)
{
    ///if texture is loaded in cache, copy only pointer to texture data
    if(cache != -1)
    {
        m_texID = cache;
    }
    else      //load texture from file
    {
        //texture generation
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);

        //load 6 images for cube map
        for(int i = 0; i < 6; i++)
        {
            if(!LoadImage(cube_files[i]))
                return ERR;

            //texture with anisotropic filtering
            if(aniso)
            {
                //find out, if GFX supports aniso filtering
                if(!GLEW_EXT_texture_filter_anisotropic)
                {
                    cout<<"Anisotropic filtering not supported. Using linear instead.\n";
                }
                else
                {
                    float maxAnisotropy;
                    //find out maximum supported anisotropy
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
                    glTexParameterf(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
                }
            }

#ifdef _LINUX_
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGB, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_imageData);
#else
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData);
#endif

            delete [] m_imageData;      //don't need image data after texture was created
        }
    }

    //texture mode and type
    m_texname = texname;
    m_textype = textype;
    m_texmode = texmode;
    m_tileX = tileX;
    m_tileY = tileY;
    m_intensity = intensity;

    //cout<<"Texture \""<<texname<<"\" creation OK\n";

    return m_texID;
}

/**
****************************************************************************************************
@brief Create picture directly from data
@param texname texture name
@param textype texture type (can be BASE,ENV,BUMP)
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
GLint Texture::Load(const char *texname, int textype, const void *texdata, glm::vec2 tex_size, GLenum tex_format, GLenum data_type,
                    int texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso)
{
    m_width = (int)tex_size.x;
    m_height = (int)tex_size.y;

    //texture generation
    glBindTexture(GL_TEXTURE_2D, m_texID);

    //texture with anisotropic filtering
    if(aniso)
    {
        //find out, if GFX supports aniso filtering
        if(!GLEW_EXT_texture_filter_anisotropic)
            cout<<"Anisotropic filtering not supported. Using linear instead.\n";
        else
        {
            float maxAnisotropy;
            //find out maximum supported anisotropy
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //mip-mapped texture (if set)
    if(mipmap)
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

#ifdef _LINUX_
        glTexImage2D(GL_TEXTURE_2D, 0, tex_format, m_width, m_height, 0, GL_BGRA, data_type, texdata);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, tex_format, m_width, m_height, 0, GL_RGBA, data_type, texdata);
#endif

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if(mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    //texture mode and type
    m_texname = texname;
    m_textype = textype;
    m_texmode = texmode;
    m_tileX = tileX;
    m_tileY = tileY;
    m_intensity = intensity;

    //cout<<"Texture \""<<texname<<"\" creation OK\n";

    return m_texID;
}


/**
****************************************************************************************************
@brief load and decode TGA image from file
@param filename file with image data
@return success/fail of texture loading
****************************************************************************************************/
bool Texture::LoadImage(const char* filename)
{
	if(!filename)
		return false;

	//Opens image
	if(!Texture::isILInitialized)
	{
		ilInit();
		Texture::isILInitialized = true;
	}

	ILuint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	if(!ilLoadImage(filename))
	{
		ShowMessage("Cannot open texture file!");
		return false;
	}

	//Get image attributes
	m_width = ilGetInteger(IL_IMAGE_WIDTH);
	m_height = ilGetInteger(IL_IMAGE_HEIGHT);
	m_bpp = ilGetInteger(IL_IMAGE_BPP);

	if((m_width <= 0) || (m_height <= 0) )
	{
		ShowMessage("Unknown image type!");
		return false;
	}

	//Allocate memory for image
	unsigned imageSize = (m_bpp * m_width * m_height);
	m_imageData = new GLubyte[imageSize];

	if(m_imageData == NULL)
	{
		ShowMessage("Can't allocate image data!");
		return false;
	}

	//Copy pixels
	if(m_bpp==1)
		ilCopyPixels(0, 0, 0, m_width, m_height, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, m_imageData);
	else if(m_bpp==3)
		ilCopyPixels(0, 0, 0, m_width, m_height, 1, IL_RGB, IL_UNSIGNED_BYTE, m_imageData);
	else if(m_bpp==4)
		ilCopyPixels(0, 0, 0, m_width, m_height, 1, IL_RGBA, IL_UNSIGNED_BYTE, m_imageData);

	//Unbind and free
	ilBindImage(0);
	ilDeleteImage(id);

	cout<<"Image loaded: "<< filename <<"\n";
	return true;
}


/**
****************************************************************************************************
@brief Activates texture map for use by shader
@param tex_unit multitexture unit used for texture application
@param set_uniforms shall we also set uniform locations to shader?
****************************************************************************************************/
void Texture::ActivateTexture(GLint tex_unit, bool set_uniforms)
{
	///1. set uniform values in shader (tiles, texture unit, texture matrix and intensity)
	if(set_uniforms)
	{
		if(m_tileXLoc > 0 && m_tileYLoc > 0)
		{
			glUniform1f(m_tileXLoc, m_tileX);
			glUniform1f(m_tileYLoc, m_tileY);
		}
		glUniform1f(m_intensityLoc, m_intensity);
	}
	//texture location must be updated regularly
	if(m_texLoc >= 0)
		glUniform1i(m_texLoc, tex_unit);

	///2. activate and bind texture
	glActiveTexture(GL_TEXTURE0 + tex_unit);
	//Various texture targets
	if(m_textype == CUBEMAP || m_textype == CUBEMAP_ENV)        //for cube map
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
	else if(m_textype == SHADOW_OMNI)                         //for texture array
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_texID);
	else if(m_textype == RENDER_TEXTURE_MULTISAMPLE)          //for multisampled texture
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_texID);
	else                                                    //for regular 2D texture
		glBindTexture(GL_TEXTURE_2D, m_texID);
}

/**
****************************************************************************************************
@brief Gets uniform variables froms shader
@param shader handle to shader to bound with texture
****************************************************************************************************/
void Texture::GetUniforms(GLuint shader)
{
    if(HasTiles())
    {
        string tileX_str = m_texname + "_tileX";
        string tileY_str = m_texname + "_tileY";
        m_tileXLoc = glGetUniformLocation(shader,tileX_str.c_str() );
        m_tileYLoc = glGetUniformLocation(shader,tileY_str.c_str() );
    }

    ///1. to avoid mismatch variable names, use texture name as variable prefix
    string intensity_str = m_texname + "_intensity";

    ///2. get uniforms location
    m_texLoc = glGetUniformLocation(shader,m_texname.c_str());
    m_intensityLoc = glGetUniformLocation(shader,intensity_str.c_str() );
}

