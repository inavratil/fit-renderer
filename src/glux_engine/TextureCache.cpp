#include "TextureCache.h"

TextureCache * TextureCache::Instance()
{
    if (m_pInstance == 0)
        m_pInstance = new TextureCache();

    return static_cast<TextureCache *>(m_pInstance);
}

TextureCache::TextureCache(void) :
	Singleton()
{
}


TextureCache::~TextureCache(void)
{
}
