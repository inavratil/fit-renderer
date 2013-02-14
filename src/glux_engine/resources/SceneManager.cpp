#include "SceneManager.h"

SceneManager * SceneManager::m_pInstance = 0;

SceneManager * SceneManager::Instance()
{
    if (m_pInstance == 0)
        m_pInstance = new SceneManager();

    return m_pInstance;
}

void SceneManager::Destroy()
{
    if (m_pInstance != 0)
        delete m_pInstance;

    m_pInstance = 0;
}

SceneManager::SceneManager(void)
{
}


SceneManager::~SceneManager(void)
{
}
