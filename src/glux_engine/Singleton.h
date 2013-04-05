#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template< typename T > class Singleton
{

protected:
	static T * m_pInstance;

public:
	Singleton(void){ assert( !m_pInstance ); m_pInstance = static_cast<T*>(this); }
	~Singleton(void){ assert( m_pInstance ); m_pInstance = 0; }


	static T * Instance()
	{     
		if (m_pInstance == 0)
			m_pInstance = new T();

		return m_pInstance; 
	}
	
    static void Destroy()
	{    
		if (m_pInstance != 0)
			delete m_pInstance;
		m_pInstance = 0;
	}
};

#endif

