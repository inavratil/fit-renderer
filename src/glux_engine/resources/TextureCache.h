#ifndef _TEXTURECACHE_H_
#define _TEXTURECACHE_H_

#include "globals.h"
#include "Singleton.h"
#include "texture.h"

class TextureCache : public Singleton<TextureCache>
{ 

//-----------------------------------------------------------------------------
//-- Members

protected:
	///associative array with all textures
	map<string,TexturePtr> m_textureCache;
    ///iterator for texture cache container
    map<string,TexturePtr>::iterator m_itTextureCache;

//-----------------------------------------------------------------------------
//-- Public methods

public:
	TextureCache(void);
	virtual ~TextureCache(void);

	TexturePtr CreateTexture( GLenum _target, GLenum _filter, bool _mipmaps );
	GLuint Create2DManual( 
		const char* _name, 
		int _resX, int _resY,
		GLint _internalFormat,
		GLenum _dataType, 
		GLenum _filter,
		bool _mipmaps
		);
	GLuint Create2DArrayManual( 
		const char* _name, 
		int _resX, int _resY,
		int _numLayers,
		GLint _internalFormat,
		GLenum _dataType, 
		GLenum _filter,
		bool _mipmaps
		);
	
	void Add( const char* _name, TexturePtr _tex );
	GLuint Get( const char* _name );
	TexturePtr GetPtr( const char* _name );

};

typedef TextureCache* TextureCachePtr;

#endif

