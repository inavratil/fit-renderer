/**
****************************************************************************************************
****************************************************************************************************
@file light.h
@brief light class and light settings
****************************************************************************************************
***************************************************************************************************/
#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "globals.h"
#include "object.h"

///@class TLight 
///@brief contains light preferences like color,position,shadow...
class TLight
{
private:
    glm::vec3 m_pos, m_ambient, m_diffuse, m_specular;     //light settings
    GLfloat m_radius;
    GLint m_light;        //light number
    bool m_shadow;        //has light shadow?

    //shadow parameters
    GLint m_shadow_size;      //shadow map size
    GLfloat m_shadow_intensity;  //intensity of shadow
    int m_light_type;        //shadow type (spot, omni)
    GLuint m_shadow_tex;    //shadow texture

public:
    ///to calculate shadows, we need framebuffer for shadow map
    GLuint fbo;       

    TLight();
    ~TLight();
    /**
    @brief Main constructor, directly create light from parameters
    @param lights light index
    @param amb ambient light color
    @param diff diffuse light color
    @param spec specular light color
    @param lpos vector with light position
    @param radius light attenuation radius
    ***********************************************************************************/
    TLight(GLint lights, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 lpos, GLfloat radius){
        Set(lights,amb,diff,spec,lpos,radius); 
    }

    ///@brief Move light to new absolute position
    void Move(glm::vec3 w){ 
        m_pos = w; 
    }

    //change color
    void ChangeColor(GLint component, glm::vec3 color);
    //set all light settings
    void Set(GLint lights, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 lpos, GLfloat radius);
    //sets light shadow
    void SetShadow(GLint shadow_size, GLfloat _shadow_intensity, int type, bool shadow);
    ///@brief set light radius
    void SetRadius(GLfloat value){ 
        m_radius = value; 
    }
    ///@brief Return light position
    glm::vec3 GetPos(){ 
        return m_pos; 
    }
    ///@brief Get light number
    GLint GetOrd(){ 
        return m_light; 
    }
    ///@brief Get light radius
    GLfloat GetRadius(){ 
        return m_radius; 
    }
    ///@brief Get light color. Component can be ambient,diffuse,specular
    glm::vec3 GetColor(int component){
        switch(component)
        {
        case AMBIENT: return m_ambient;
        case DIFFUSE: return m_diffuse;
        case SPECULAR: return m_specular;
        default: return glm::vec3(0.0);
        }
    }

    ///////////////// SHADOWS //////////////////////////
    ///@brief Returns light shadow
    bool HasShadow(){ 
        return m_shadow; 
    } 
    ///@brief return shadow map size
    GLint ShadowSize(){ 
        return m_shadow_size; 
    }  
    ///@brief return shadow intensity
    GLfloat ShadowIntensity(){ 
        return m_shadow_intensity; 
    } 
    ///@brief Get FBO for shadow map rendering
    GLuint GetFBO(){ 
        return fbo; 
    }
    ///@brief get light type
    int GetType(){ 
        return m_light_type; 
    }
    ///@brief Set shadow cache
    void SetShadowTexID(GLuint texid){
        m_shadow_tex = texid;
    }
    ///@brief Get shadow cache
    GLuint* GetShadowTexID(){
        return &m_shadow_tex;
    }
};

#endif
