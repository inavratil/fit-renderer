#ifndef _TEXTURECACHE_H_
#define _TEXTURECACHE_H_

#include "globals.h"

class TextureCache : public Singleton<TextureCache>
{ 

//-- Members

protected:
	//static TextureCache * m_pInstance;
	///associative array with all textures
	map<string,GLuint> m_textureCache;
    ///iterator for texture cache container
    map<string,GLuint>::iterator m_itTextureCache;

//-- Static methods 

public:
    //static TextureCache * Instance();
	//static void Destroy();


//-- Public methods

public:
	TextureCache(void);
	virtual ~TextureCache(void);

	void Add( const char* _name, int _resX, int _resY, GLint _internalFormat, GLenum _dataType, GLenum _target = GL_TEXTURE_2D, bool _mipmaps = false);
	void Add( const char* _name, GLuint _id );
	GLuint Get( const char* _name );

	void SetFilter( const char* _name, GLfloat  _filter );
};

#endif

