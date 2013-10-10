#include "MaterialManager.h"

template<> MaterialManager * Singleton<MaterialManager>::m_pInstance = 0;

//-----------------------------------------------------------------------------

MaterialManager::MaterialManager()
{
	_Init();
}

//-----------------------------------------------------------------------------

MaterialManager::~MaterialManager(void)
{
	_Destroy();
}

//-----------------------------------------------------------------------------

void MaterialManager::_Init()
{
}

//-----------------------------------------------------------------------------

void MaterialManager::_Destroy()
{
	//free all objects, materials, textures...
    for(m_im = m_materials.begin(); m_im != m_materials.end(); m_im++)
        delete m_im->second;
	m_materials.clear();
}

//-----------------------------------------------------------------------------
Material* MaterialManager::GetMaterial( const char* _name )
{
	if(m_materials.find(_name) == m_materials.end())
		cerr<<"ERROR (GetMaterial): no material with name "<<_name<<"\n";
	return m_materials[_name];
}

//-----------------------------------------------------------------------------

void MaterialManager::AddMaterial( Material* _mat )
{
	if( !_mat ) return;
	//NAVY_FIX _mat->SetSceneID( m_scene->GetSceneID() );
	_mat->SetID( m_materials.size() );
	m_materials[_mat->GetName()] = _mat;
	//NAVY_FIX m_scene->UpdateLoadList( 1 );
	cout<<"Adding material "<<_mat->GetName()<<endl;
}

//-----------------------------------------------------------------------------

///@brief Add new material(see GeometryMaterial()) to list
void MaterialManager::AddMaterial(const char* name, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec,
	GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm){
		GeometryMaterial *m = new GeometryMaterial(name, -1, amb, diff, spec, shin, reflect, transp, lm);
		//NAVY_FIX m->SetSceneID( m_scene->GetSceneID() );
		m->SetID( m_materials.size() );
		m_materials[name] = m;
		//NAVY_FIX m_scene->UpdateLoadList( 1 );
		cout<<"Adding material "<<name<<endl;
}

//-----------------------------------------------------------------------------
