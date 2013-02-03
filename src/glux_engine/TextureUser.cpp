#include "TextureUser.h"


TextureUser::TextureUser(void)
{
}


TextureUser::~TextureUser(void)
{
}

void TextureUser::AddTexture( string _name )
{
	m_textureList.push_back( _name );
}
