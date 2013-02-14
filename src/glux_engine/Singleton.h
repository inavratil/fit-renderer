#ifndef _SINGLETON_H_
#define _SINGLETON_H_

class Singleton
{

protected:
	static Singleton * m_pInstance;

public:
	Singleton(void);
	virtual ~Singleton(void);

	static Singleton * Instance();
    static void Destroy();

};

#endif

