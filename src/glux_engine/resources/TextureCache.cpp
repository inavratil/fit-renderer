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

TexturePtr TextureCache::CreateTexture( GLenum _target, GLenum _filter, bool _mipmaps )
{
	//create texture
	TexturePtr tex = new Texture( _target );

	//-- set up filtering
    GLenum filter = _filter;
    if(_mipmaps)
	{
		if( filter == GL_NEAREST )
			filter = GL_NEAREST_MIPMAP_NEAREST;
		else if( filter == GL_LINEAR )
			filter = GL_NEAREST_MIPMAP_LINEAR;
	}
	
	//following properties can be set only in normal 2D textures
	tex->SetFiltering( filter );	
	tex->SetWrap( GL_CLAMP_TO_EDGE );

	return tex;
}
//-----------------------------------------------------------------------------

GLuint TextureCache::Create2DManual(const char* _name, int _resX, int _resY, GLint _internalFormat, GLenum _dataType, GLenum _filter, bool _mipmaps )
{
	TexturePtr tex = CreateTexture( TEX_2D, _filter, _mipmaps );

	GLenum format = GL_RGBA;
	if( _internalFormat == GL_DEPTH_COMPONENT )
		format = GL_DEPTH_COMPONENT;

	tex->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _resX, _resY, 0, format, _dataType, NULL);

	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

    //add created textures into cache
    Add( _name, tex );

	tex->Unbind();

	return tex->GetID();
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
	TexturePtr tex = CreateTexture( TEX_2D_ARRAY, _filter, _mipmaps );

	GLenum format = GL_RGBA;
	if( _internalFormat == GL_DEPTH_COMPONENT )
		format = GL_DEPTH_COMPONENT;

	tex->Bind();
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, _internalFormat, _resX, _resY, _numLayers, 0, format, _dataType, NULL);

	if(_mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    //add created textures into cache
    Add( _name, tex );

	tex->Unbind();

	return tex->GetID();
}

//-----------------------------------------------------------------------------

void TextureCache::Add( const char* _name, TexturePtr _tex )
{
	if( m_textureCache.find(_name) != m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): texture with name "<<_name<<" already exist\n";
		return;
	}

	m_textureCache[_name] = _tex;
}

//-----------------------------------------------------------------------------

GLuint TextureCache::Get( const char* _name )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
		return 0;
	}

	//FIXME: muze vracet blbosti
	return m_textureCache[_name]->GetID();
}

//-----------------------------------------------------------------------------

TexturePtr TextureCache::GetPtr( const char* _name )
{
	if( m_textureCache.find(_name) == m_textureCache.end() )
	{
		cerr<<"WARNING (TextureCache): no texture with name "<<_name<<"\n";
	}

	//FIXME: muze vracet blbosti
	return m_textureCache[_name];
}

//-----------------------------------------------------------------------------