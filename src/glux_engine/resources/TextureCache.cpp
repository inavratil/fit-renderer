#include "TextureCache.h"

TextureCache * TextureCache::m_pInstance = 0;

TextureCache * TextureCache::Instance()
{
    if (m_pInstance == 0)
        m_pInstance = new TextureCache();

    return m_pInstance;
}

void TextureCache::Destroy()
{
    if (m_pInstance != 0)
        delete m_pInstance;

    m_pInstance = 0;
}

TextureCache::TextureCache(void)
{
}


TextureCache::~TextureCache(void)
{
}

void TextureCache::Add(const char* _name, int _resX, int _resY, GLint _internalFormat, GLenum _dataType, GLenum _target, bool _mipmaps)
{
	//create texture
    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(_target, texid);

    GLenum filter = GL_NEAREST;
    if(_mipmaps)
        filter = GL_NEAREST_MIPMAP_NEAREST;

	//following properties can be set only in normal 2D textures
	glTexParameterf(_target, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(_target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameterf(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _resX, _resY, 0, GL_RGBA, _dataType, NULL);
	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

    //add created textures into cache
    m_textureCache[_name] = texid;

	glBindTexture(_target, 0);
}

GLuint TextureCache::Get( const char* _name )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
		return 0;
	}

	return m_textureCache[_name];
}

void TextureCache::SetFilter( const char* _name, GLfloat  _filter )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
		return;
	}

	glBindTexture( GL_TEXTURE_2D, m_textureCache[_name] );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter);

	glBindTexture( GL_TEXTURE_2D, 0 );
}
