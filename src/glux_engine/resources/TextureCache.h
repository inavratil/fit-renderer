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
	map<string,TexturePtr> m_texture_cache;
    ///iterator for texture cache container
    map<string,TexturePtr>::iterator m_it_texture_cache;

	bool	m_is_IL_Initialized;

//-----------------------------------------------------------------------------
//-- Public methods

public:
	TextureCache(void);
	virtual ~TextureCache(void);

	TexturePtr Create2DManual( 
		const char* _name, 
		int _resX, int _resY,
		GLint _internalFormat,
		GLenum _dataType, 
		GLenum _filter,
		bool _mipmaps
		);
	TexturePtr Create2DArrayManual( 
		const char* _name, 
		int _resX, int _resY,
		int _numLayers,
		GLint _internalFormat,
		GLenum _dataType, 
		GLenum _filter,
		bool _mipmaps
		);
	TexturePtr CreateFromImage( const char* _filename );

	void Add( const char* _name, TexturePtr _tex );
	GLuint Get( const char* _name );
	TexturePtr GetPtr( const char* _name );

};

typedef TextureCache* TextureCachePtr;

#endif

