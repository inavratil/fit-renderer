#ifndef _TEXTURECACHE_H_
#define _TEXTURECACHE_H_

#include "globals.h"
#include "Singleton.h"

class TextureCache : public Singleton<TextureCache>
{ 

//-----------------------------------------------------------------------------
//-- Members

protected:
	///associative array with all textures
	map<string,GLuint> m_textureCache;
    ///iterator for texture cache container
    map<string,GLuint>::iterator m_itTextureCache;

//-----------------------------------------------------------------------------
//-- Public methods

public:
	TextureCache(void);
	virtual ~TextureCache(void);

	GLuint CreateTexture( GLenum _target, GLenum _filter, bool _mipmaps );
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
	
	void Add( const char* _name, GLuint _id );
	GLuint Get( const char* _name );

};

typedef TextureCache* TextureCachePtr;

#endif

