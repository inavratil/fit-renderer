#ifndef _TEXTURECACHE_H_
#define _TEXTURECACHE_H_

#include "globals.h"

class TextureCache : public Singleton
{ 

//-- Members

protected:
	///associative array with all textures
	map<string,GLuint> m_textureCache;
    ///iterator for texture cache container
    map<string,GLuint>::iterator m_itTextureCache;

//-- Static methods 

public:
    static TextureCache * Instance();


//-- Public methods

public:
	TextureCache(void);
	virtual ~TextureCache(void);

	void AddTexture( string _name, GLuint _id );
};

#endif

