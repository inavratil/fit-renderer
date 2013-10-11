#ifndef _MATERIALMANAGER_H_
#define _MATERIALMANAGER_H_

#include "globals.h"
#include "sdk/Singleton.h"
#include "sdk/Material.h"
#include "sdk/GeometryMaterial.h"

#define DECL_MATERIAL( c ) \
	MaterialPtr DEFAULT_##c##(){ return m_materials["DEFAULT_##c##"]; } \
	int DEFAULT_##c##_ID(){ return m_materials["DEFAULT_##c##"]->GetID(); }

class MaterialManager : public Singleton<MaterialManager>
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:	    
	//-- associative array with all materials
    map<string,Material*> m_materials;
    //-- iterator for materials container
    map<string,Material*>::iterator m_im;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	MaterialManager();
	virtual ~MaterialManager(void);

	Material* GetMaterial( const char* _name );
	void AddMaterial( Material* _mat );
    ///@brief Add new material(see GeometryMaterial()) to list
    void AddMaterial(const char* name, glm::vec3 amb = black, glm::vec3 diff = silver, glm::vec3 spec = white,
        GLfloat shin = 64.0, GLfloat reflect = 0.0, GLfloat transp = 0.0, GLint lm = PHONG);

	//-- (Something like) Iterator pattern
	void Begin(){ m_im = m_materials.begin(); }
	void Next(){ m_im++; }
	bool End(){	return m_im == m_materials.end(); }
	MaterialPtr GetItem(){ return m_im->second; }

	//-- Get default colored materials	
	DECL_MATERIAL( RED )
	DECL_MATERIAL( GREEN )
	DECL_MATERIAL( BLUE )
	DECL_MATERIAL( SILVER )

//-----------------------------------------------------------------------------
//-- Private methods

private:
	void _Init();
	void _Destroy();

};

//-----------------------------------------------------------------------------
//-- Inline methods 

typedef MaterialManager* MaterialManagerPtr;

#endif

