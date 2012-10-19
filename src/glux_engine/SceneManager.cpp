#include "SceneManager.h"

SceneManager * SceneManager::Instance()
{
    if (m_pInstance == 0)
        m_pInstance = new SceneManager();

    return static_cast<SceneManager *>(m_pInstance);
}

SceneManager::SceneManager(void) :
	Singleton()
{
}


SceneManager::~SceneManager(void)
{
}
