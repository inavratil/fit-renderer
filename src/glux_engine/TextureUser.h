#ifndef _TEXTUREUSER_H_
#define _TEXTUREUSER_H_

#include "globals.h"

class TextureUser
{ 

//-- Members

protected:
	//-- list of all textures
	vector<string> m_textureList;
    //-- iterator for texture list
    vector<string>::iterator m_itTextureCache;

//-- Public methods

public:
	TextureUser(void);
	virtual ~TextureUser(void);

	void AddTexture( string _name );
};

#endif

