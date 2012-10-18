#include "Singleton.h"


Singleton * Singleton::m_pInstance = 0;

//-- Static methods
Singleton * Singleton::Instance()
{
    if (m_pInstance == 0)
        m_pInstance = new Singleton();

    return m_pInstance;
}

void Singleton::Destroy()
{
    if (m_pInstance != 0)
        delete m_pInstance;

    m_pInstance = 0;
}

Singleton::Singleton(void)
{
}


Singleton::~Singleton(void)
{
}
