#include "FBOManager.h"

template<> FBOManager * Singleton<FBOManager>::m_pInstance = 0;

FBOManager::FBOManager(void)
{
}


FBOManager::~FBOManager(void)
{
}

void FBOManager::Add( const char* _name, GLuint _id )
{
	m_fbos[_name] = _id;
}

GLuint FBOManager::Get( const char* _name )
{
	if( m_fbos.find(_name) == m_fbos.end() )
	{
		cerr<<"WARNING (FBOManager): no fbo with name "<<_name<<"\n";
		return 0;
	}

	return m_fbos[_name];
}
