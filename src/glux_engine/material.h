#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "texture.h"

class Material
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	//-- properties
	string	m_name;
    int		m_id;
    int		m_sceneID;	//-- scene ID - when drawing more scenes than 1
	
	//-- associative array of input textures
	map<string,Texture*>			m_textures;		//-- textures list    
    map<string,Texture*>::iterator	m_it_textures;	//-- texture iterator

	//-- shader
	//FIXME: not used	string m_source;          //custom shader source
	//FIXME: not used	map<const char*,GLint> m_shader_locations;
    //FIXME: not used	GLint m_sh_loc;
    GLint m_f_shader, m_tc_shader, m_te_shader, m_g_shader, m_v_shader, m_shader;
    

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Material( const char* _name );
	Material( const char* _name, int _id );
	virtual ~Material(void);

//-----------------------------------------------------------------------------
	//-- Set/Get material name
	void SetName( const char* _name ){ m_name = _name; }
    string GetName(){ return m_name; }
    //-- Set/Get material ID
	void SetID( int _id ){ m_id = _id; }
    int GetID(){ return m_id; }
    //--Set/Get scene ID
    int GetSceneID(){ return m_sceneID; }
    void SetSceneID(int _id){ m_sceneID = _id; }

//-----------------------------------------------------------------------------
    ///@brief Set float uniform value in shader
    void SetUniform(const char* v_name, float value){
        glProgramUniform1f(m_shader, glGetUniformLocation(m_shader,v_name), value);
    }
    ///@brief Set double uniform value in shader. It is treated like float
    void SetUniform(const char* v_name, double value){
        glProgramUniform1f(m_shader, glGetUniformLocation(m_shader,v_name), (float)value);
    }
    ///@brief Set int uniform value in shader
    void SetUniform(const char* v_name, int value){
        glProgramUniform1i(m_shader, glGetUniformLocation(m_shader,v_name), value);
    }
    ///@brief Set ivec2 value
    void SetUniform(const char* v_name, glm::ivec2 value){
        glProgramUniform2iv(m_shader, glGetUniformLocation(m_shader,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set vec2 value
    void SetUniform(const char* v_name, glm::vec2 value){
        glProgramUniform2fv(m_shader, glGetUniformLocation(m_shader,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set vec3 value
    void SetUniform(const char* v_name, glm::vec3 value){
        glProgramUniform3fv(m_shader, glGetUniformLocation(m_shader,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set vec4 value
    void SetUniform(const char* v_name, glm::vec4 value){
        glProgramUniform4fv(m_shader, glGetUniformLocation(m_shader,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set mat4x4 value
    void SetUniform(const char* v_name, glm::mat4 &value){
        glProgramUniformMatrix4fv(m_shader, glGetUniformLocation(m_shader,v_name), 1, 0, glm::value_ptr(value));
    }


};

typedef Material* MaterialPtr;
#endif

