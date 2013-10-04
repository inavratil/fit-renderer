#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#include "globals.h"
#include "object.h"
#include "sdk\Singleton.h"

enum VBOMembers { VBO_ARRAY, VBO_BUFFER, VBO_INDEX };

class SceneManager : public Singleton<SceneManager>
{ 

//-- Member variables

protected:
	///associative array with all VBOs
	map<string,VBO> m_vbos;
    ///iterator for vbos container
    map<string,VBO>::iterator m_ivbo;

//-- Public methods

public:
	SceneManager(void);
	virtual ~SceneManager(void);

	void setVBO( string _name, VBO _vbo )
	{
		m_vbos[_name] = _vbo;
	}

	///@brief Return given type of VBO according to its name. The third parameter is used as index for VBO_BUFFER type
	GLuint getVBO( VBOMembers _type, string _name, int _i = 0 )
	{
		assert(_i<4);

		GLuint ret;

		switch(_type)
		{
		case VBO_ARRAY:
			ret = m_vbos[_name].vao;
			break;
		case VBO_BUFFER:
			ret = m_vbos[_name].buffer[_i];
			break;
		case VBO_INDEX:
			ret = m_vbos[_name].indices;
			break;
		default:
			ret = 0;
			break;
		}
		
		return ret;
	}

};

#endif

