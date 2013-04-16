#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "texture.h"

///Aligned buffer size
#define BUFFER 512

///@struct TShader
///@brief structure for shader properties
struct TShader
{
    int type;
    string source;
    string defines;

    TShader(const char* s, const char* d){
        type = FRAGMENT;
        source = s;
        defines = d;
    }
};

class Material
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	//-- properties
	string	m_name;
    int		m_id;
    int		m_sceneID;	//-- scene ID - when drawing more scenes than 1
	bool	m_is_screenspace;
	bool	m_has_alpha_channel;
	
	//-- associative array of input textures
	map<string,Texture*>			m_textures;		//-- textures list    
    map<string,Texture*>::iterator	m_it_textures;	//-- texture iterator

	//-- shader
	bool	m_has_tessellation_shader;
	//FIXME: not used	string m_source;          //custom shader source
	//FIXME: not used	map<const char*,GLint> m_shader_locations;
    //FIXME: not used	GLint m_sh_loc;
    GLint m_f_shader, m_tc_shader, m_te_shader, m_g_shader, m_v_shader, m_program;
    

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Material( const char* _name );
	Material( const char* _name, int _id );
	virtual ~Material(void);

	string NextTexture( string _texname );
	GLuint AddTexturePtr( TexturePtr _tex );
	void RemoveTexture( const char *_texName );

	string LoadShader( const char* _filename );

	
    //dynamically generate material shader
	virtual bool BakeMaterial(int light_count, int dpshadow_method = DPSM, bool use_pcf = true){  return true; }
	virtual int RenderMaterial();

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


	//-- Is shader using tessellation stage?
    bool HasTessellationShader(){  
        return m_has_tessellation_shader;  
    }
	//-- Has shader material alpha channel?
    bool IsAlpha(){  
        return m_has_alpha_channel; 
    }
	//-- is material working in screen space?
    bool IsScreenSpace(){  
        return m_is_screenspace; 
    }
    ///Has material valid shader?
    bool IsShaderOK(){
        return (m_program > 0);
    }

//-----------------------------------------------------------------------------
    ///@brief Set float uniform value in shader
    void SetUniform(const char* v_name, float value){
        glProgramUniform1f(m_program, glGetUniformLocation(m_program,v_name), value);
    }
    ///@brief Set double uniform value in shader. It is treated like float
    void SetUniform(const char* v_name, double value){
        glProgramUniform1f(m_program, glGetUniformLocation(m_program,v_name), (float)value);
    }
    ///@brief Set int uniform value in shader
    void SetUniform(const char* v_name, int value){
        glProgramUniform1i(m_program, glGetUniformLocation(m_program,v_name), value);
    }
    ///@brief Set ivec2 value
    void SetUniform(const char* v_name, glm::ivec2 value){
        glProgramUniform2iv(m_program, glGetUniformLocation(m_program,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set vec2 value
    void SetUniform(const char* v_name, glm::vec2 value){
        glProgramUniform2fv(m_program, glGetUniformLocation(m_program,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set vec3 value
    void SetUniform(const char* v_name, glm::vec3 value){
        glProgramUniform3fv(m_program, glGetUniformLocation(m_program,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set vec4 value
    void SetUniform(const char* v_name, glm::vec4 value){
        glProgramUniform4fv(m_program, glGetUniformLocation(m_program,v_name), 1, glm::value_ptr(value));
    }
    ///@brief Set mat4x4 value
    void SetUniform(const char* v_name, glm::mat4 &value){
        glProgramUniformMatrix4fv(m_program, glGetUniformLocation(m_program,v_name), 1, 0, glm::value_ptr(value));
    }

//-----------------------------------------------------------------------------
private:
	void _Init( const char* _name, int _id );


};

typedef Material* MaterialPtr;
#endif

