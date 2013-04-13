#include "TextureCache.h"

template<> TextureCache * Singleton<TextureCache>::m_pInstance = 0;

//-----------------------------------------------------------------------------

TextureCache::TextureCache(void)
{
}

//-----------------------------------------------------------------------------

TextureCache::~TextureCache(void)
{
}

//-----------------------------------------------------------------------------

GLuint TextureCache::CreateTexture( GLenum _target, GLenum _filter, bool _mipmaps )
{
	//create texture
    GLuint texid;
    glGenTextures(1, &texid);

	//-- set up filtering
    GLenum filter = _filter;
    if(_mipmaps)
	{
		if( filter == GL_NEAREST )
			filter = GL_NEAREST_MIPMAP_NEAREST;
		else if( filter == GL_LINEAR )
			filter = GL_NEAREST_MIPMAP_LINEAR;
	}
	
	//FIXME: Leave the texture bound!
    glBindTexture(_target, texid);

	//following properties can be set only in normal 2D textures
	glTexParameterf(_target, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(_target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameterf(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texid;
}
//-----------------------------------------------------------------------------

GLuint TextureCache::Create2DManual(const char* _name, int _resX, int _resY, GLint _internalFormat, GLenum _dataType, GLenum _filter, bool _mipmaps )
{
	GLuint texid = CreateTexture( GL_TEXTURE_2D, _filter, _mipmaps );

	GLenum format = GL_RGBA;
	if( _internalFormat == GL_DEPTH_COMPONENT )
		format = GL_DEPTH_COMPONENT;

	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _resX, _resY, 0, format, _dataType, NULL);

	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

    //add created textures into cache
    Add( _name, texid );

	glBindTexture(GL_TEXTURE_2D, 0);

	return texid;
}

//-----------------------------------------------------------------------------
	
GLuint TextureCache::Create2DArrayManual( 
	const char* _name, 
	int _resX, int _resY,
	int _numLayers,
	GLint _internalFormat,
	GLenum _dataType, 
	GLenum _filter,
	bool _mipmaps
	)
{
	GLuint texid = CreateTexture( GL_TEXTURE_2D_ARRAY, _filter, _mipmaps );

	GLenum format = GL_RGBA;
	if( _internalFormat == GL_DEPTH_COMPONENT )
		format = GL_DEPTH_COMPONENT;

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _internalFormat, _resX, _resY, _numLayers, 0, format, _dataType, NULL);

	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    //add created textures into cache
    Add( _name, texid );

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return texid;
}

//-----------------------------------------------------------------------------

void TextureCache::Add( const char* _name, GLuint _id )
{
	if( m_textureCache.find(_name) != m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): texture with name "<<_name<<" already exist\n";
		return;
	}

	m_textureCache[_name] = _id;
}

//-----------------------------------------------------------------------------

GLuint TextureCache::Get( const char* _name )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
		return 0;
	}

	return m_textureCache[_name];
}

//-----------------------------------------------------------------------------
