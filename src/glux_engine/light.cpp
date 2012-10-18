/**
****************************************************************************************************
****************************************************************************************************
@file light.cpp
@brief light class and light settings - implementation
****************************************************************************************************
***************************************************************************************************/
#include "light.h"

/**
****************************************************************************************************
@brief Make empty light: zero index, position at 0,0,0, without shadow
***************************************************************************************************/
TLight::TLight()
{
    m_light = 0;
    m_pos = glm::vec3(0.0);
    m_shadow = false;
    fbo = 0;
    m_light_type = SPOT;
}

/**
****************************************************************************************************
@brief Delete dynamic data
***************************************************************************************************/
TLight::~TLight()
{
    ///delete framebuffer
    if(fbo != 0)
        glDeleteFramebuffers(1, &fbo);
}

/**
****************************************************************************************************
@brief Change light's component color
@param component light color component (can be AMBIENT,DIFFUSE,SPECULAR)
@param color color (as RGB TVector)
***************************************************************************************************/
void TLight::ChangeColor(GLint component, glm::vec3 color)
{
    switch(component)
    {
    case AMBIENT:
        m_ambient = color;
        break;
    case DIFFUSE:
        m_diffuse = color;
        break;
    case SPECULAR:
        m_specular = color;
        break;
    };
}


/**
****************************************************************************************************
@brief Set all light parameters
@param lights light index in OpenGL
@param amb ambient color (RGB TVector)
@param diff diffuse color (RGB TVector)
@param spec specular color (RGB TVector)
@param lpos light position (XYZ TVector)
@param radius light attenuation radius
***************************************************************************************************/
void TLight::Set(GLint lights, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 lpos, GLfloat radius)
{
    m_light = lights;

    //light settings
    m_ambient = amb;
    m_diffuse = diff;
    m_specular = spec;
    m_pos = lpos;
    m_radius = radius;
    m_shadow = false;

    //other
    m_light_type = SPOT;
}


/**
****************************************************************************************************
@brief Set light shadow
@param shadow_size shadow map size (must be power of two)
@param shadow_intensity mixing intensity (0 - transparent, 1 - opaque)
@param type light type (spot or omni)
@param  shadow shadows on/off
***************************************************************************************************/
void TLight::SetShadow(GLint shadow_size, GLfloat shadow_intensity, int type, bool shadow)
{
    //shadow properties
    m_shadow = shadow;
    m_shadow_size = shadow_size;
    m_light_type = type;
    m_shadow_intensity = shadow_intensity;
}

