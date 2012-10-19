#ifndef _SINGLETON_H_
#define _SINGLETON_H_

class Singleton
{
// Static
protected:
	static Singleton * m_pInstance;

public:
    static Singleton * Instance();
    static void Destroy();

public:
	Singleton(void);
	virtual ~Singleton(void);


};

#endif

