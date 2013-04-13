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
    int								m_id;
	//-- associative array of input textures
	map<string,Texture*>			m_textures;		        //textures list    
    map<string,Texture*>::iterator	m_it_textures;			 //texture iterator

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Material( const char* _name );
	Material( const char* _name, int _id );
	virtual ~Material(void);

//-----------------------------------------------------------------------------
	void SetName( const char* _name ){ m_name = _name; }
	///@brief get material name
    string GetName(){ return m_name; }
	void SetID( int _id ){ m_id = _id; }
    ///@brief get material ID
    int GetID(){ return m_id; }

};

typedef Material* MaterialPtr;
#endif

