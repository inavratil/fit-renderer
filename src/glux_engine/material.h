/**
@file material.h 
@brief material settings, dynamic shader creation
***************************************************************************************************/
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "texture.h"

///Aligned buffer size
#define BUFFER 512

///white color
const glm::vec3 white(1.0f,1.0f,1.0f);
///black color
const glm::vec3 black(0.0f,0.0f,0.0f);
///dark grey color
const glm::vec3 dgrey(0.1f,0.1f,0.1f);
///grey color
const glm::vec3 grey(0.3f,0.3f,0.3f);
///light grey color
const glm::vec3 lgrey(0.5f,0.5f,0.5f);
///silver color
const glm::vec3 silver(0.7f,0.7f,0.7f);
///red color
const glm::vec3 red(1.0f,0.0f,0.0f);
///green color
const glm::vec3 green(0.0f,1.0f,0.0f);
///blue color
const glm::vec3 blue(0.0f,0.0f,1.0f);
///yellow color
const glm::vec3 yellow(1.0f,1.0f,0.0f);
///magenta color
const glm::vec3 magenta(1.0f,0.0f,1.0f);
///cyan color
const glm::vec3 cyan(0.0f,1.0f,1.0f);

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


///@class TMaterial
///@brief hold all material properties necessary to create dynamic shader - light models textures and colors
class TMaterial
{
private:
    //properties
    string m_name;
    unsigned m_matID;
    map<string,Texture*> m_textures;          //textures list    
    map<string,Texture*>::iterator m_it;			 //texture iterator
    //material properties
    glm::vec3 m_ambColor, m_diffColor, m_specColor;
    GLfloat m_shininess, m_transparency, m_reflection;

    //shader
    string m_source;          //custom shader source
    GLint m_f_shader, m_tc_shader, m_te_shader, m_g_shader, m_v_shader, m_shader;
    map<const char*,GLint> m_shader_locations;
    GLint m_sh_loc;

    //other variables
    bool m_baked, m_custom_shader, m_receive_shadows, m_useMRT, m_is_alpha, m_is_tessellated;
    int m_lightModel;     ///lightModel - also indicates whether algorithm works in screen space

    //scene ID - when drawing more scenes than 1
    int m_sceneID;

public:
    //material creation
    TMaterial(const char* name, unsigned id, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, 
              GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm);
    //free dynamic data
    ~TMaterial();

    ///@brief get material name
    string GetName(){ 
        return m_name; 
    }
    ///@brief get material ID
    unsigned GetID(){ 
        return m_matID; 
    }
    ///@brief set transparency value
    void SetTransparency(GLfloat value){ 
        m_transparency = value; 
    }
    ///@brief get transparency value
    GLfloat GetTransparency(){ 
        return m_transparency; 
    }
    ///@brief Set material color
    void SetColor(int component, glm::vec3 color){
        switch(component){
        case AMBIENT: m_ambColor = color; break;
        case DIFFUSE: m_diffColor = color; break;
        case SPECULAR: m_specColor = color; break;
        }
    }

    //???
    GLint LoadTexture(string _texname, int _textype, const char *filename, int _texmode,
        GLfloat _intensity, GLfloat _tileX, GLfloat _tileY, bool mipmap, bool aniso, GLint cache);

    //texture from file
    GLint AddTexture(const char *file, GLint textype, GLint texmode,
                     GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso, GLint cache);
    //from cubemap
    GLint AddTexture(const char **files, GLint textype, GLint texmode,
                     GLfloat intensity, GLfloat tileX, GLfloat tileY, bool aniso, GLint cache);
    //from data
    GLint AddTextureData(const char *texname, GLint textype, const void *texdata, glm::vec2 tex_size, GLenum tex_format, GLenum data_type, 
                        GLint texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso);

    //finds next free texture in the list
    string NextTexture(string textype);   
    /**
    @brief Delete texture specified by name
    @param texname texture name
    ***************************************/
    void DeleteTexture(string texname){ 
        m_textures.erase(texname); 
    }

    /**
    @brief Change texture ID (thus change pointer to texture object)
    @param texname texture name
    @param id texture ID
    ***************************************/
    void SetTexID(string texname, GLuint id){ 
        m_textures[texname]->SetID(id); 
    }

    //add shadow map
    void AddShadowMap(int type, GLuint map, GLfloat intensity);
    //remove all shadow maps
    void RemoveShadows();


    //load custom shader - vertex and fragment stage only
    bool CustomShader(const char* vert_source, const char* frag_source, const char *vert_defines, const char *frag_defines){
        TShader vert(vert_source, vert_defines);
        TShader frag(frag_source, frag_defines);
        return CustomShader(&vert, NULL, NULL, NULL, &frag);
    }
    //load custom shader - all stages
    bool CustomShader(TShader *vertex, TShader *tess_control, TShader *tess_eval, TShader *geometry, TShader *fragment);

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

    ///@brief Toggle use of MRT
    void UseMRT(bool flag){ 
        m_useMRT = flag; 
    }

    //dynamically generate material shader
    bool BakeMaterial(int light_count, int dpshadow_method = DPSM, bool use_pcf = true);
    //render material
    void RenderMaterial();
    //is material working in screen space?
    bool IsScreenSpace(){  
        return (m_lightModel == SCREEN_SPACE); 
    }

    ///@brief toggle receiving shadows
    void ReceiveShadow(bool flag){ 
        m_receive_shadows = flag; 
    }

    ///@brief should we render material as reflective?
    GLfloat GetReflect(){ 
        return m_reflection; 
    }

    ///Get scene ID
    int GetSceneID(){  
        return m_sceneID; 
    }

    ///Set scene ID
    void SetSceneID(int id){  
        m_sceneID = id; 
    }

    ///Is shader custom?
    bool IsCustom(){  
        return m_custom_shader;  
    }

    ///Is shader using tessellation stage?
    bool IsTessellated(){  
        return m_is_tessellated;  
    }

    ///Has shader material alpha channel?
    bool IsAlpha(){  
        return m_is_alpha; 
    }
    ///Has material valid shader?
    bool IsShaderOK(){
        return (m_shader > 0);
    }

    ///Get first alpha texture ID
    GLuint GetAlphaTexID(){
        for(m_it = m_textures.begin(); m_it != m_textures.end(); ++m_it){
            if(m_it->second->GetType() == ALPHA)
                return m_it->second->GetID();
        }
        return 0;
    }
};

#endif
