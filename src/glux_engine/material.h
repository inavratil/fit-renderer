#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "texture.h"

class Material
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	//-- properties
	string							m_name;
    unsigned						m_id;
	//-- associative array of input textures
	map<string,Texture*>			m_textures;		        //textures list    
    map<string,Texture*>::iterator	m_it_textures;			 //texture iterator

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Material( const char* _name, unsigned _id );
	virtual ~Material(void);

//-----------------------------------------------------------------------------
	///@brief get material name
    string GetName(){ return m_name; }
    ///@brief get material ID
    unsigned GetID(){ return m_id; }
};

#endif

