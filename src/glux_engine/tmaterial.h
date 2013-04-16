/**
@file material.h 
@brief material settings, dynamic shader creation
***************************************************************************************************/
#ifndef _TMATERIAL_H_
#define _TMATERIAL_H_

#include "texture.h"
#include "Material.h"
#include "shaderGen/ShaderFeature.h"

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




///@class TMaterial
///@brief hold all material properties necessary to create dynamic shader - light models textures and colors
class TMaterial : public Material
{
private:  

    //material properties
    glm::vec3 m_ambColor, m_diffColor, m_specColor;
    GLfloat m_shininess, m_transparency, m_reflection;

    //other variables
    bool m_baked, m_custom_shader, m_receive_shadows, m_useMRT, m_is_alpha;
    int m_lightModel;     ///lightModel - also indicates whether algorithm works in screen space

	vector<ShaderFeature*>::iterator	m_if;
	vector<ShaderFeature*>				m_features;

public:
    //material creation
	TMaterial( const char* _name );
    TMaterial(const char* name, int id, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, 
              GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm);
    //free dynamic data
    ~TMaterial();

	void _Init( const char* name, int id, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, GLfloat shin, GLfloat reflect, GLfloat transp, GLint lm );

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
	void SetColor( glm::vec3 _amb, glm::vec3 _diff, glm::vec3 _spec )
	{
		m_ambColor = _amb;
		m_diffColor = _diff;
		m_specColor = _spec;
	}
	void SetShininess( float _shin ){ m_shininess = _shin; }

	//finds next free texture in the list
    string NextTexture(string textype);		//FIXME: do rodicovske tridy
	GLuint AddTexture( TexturePtr _tex );	//FIXME: do rodicovske tridy

    //texture from file
    GLint AddTexture(const char *file, GLint textype, GLint texmode,
                     GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso, GLint cache);
    //from cubemap
    GLint AddTexture(const char **files, GLint textype, GLint texmode,
                     GLfloat intensity, GLfloat tileX, GLfloat tileY, bool aniso, GLint cache);
    //from data
    GLint AddTextureData(const char *texname, GLint textype, const void *texdata, glm::vec2 tex_size, GLenum tex_format, GLenum data_type, 
                        GLint texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso);

	//remove texture
	void RemoveTexture( const char *_texName );

  
    /**
    @brief Delete texture specified by name
    @param texname texture name
    ***************************************/
    void DeleteTexture(string texname){ 
        m_textures.erase(texname); 
    }

    //add shadow map
    void AddTextureFromCache(int type, GLuint id, GLfloat intensity);
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

    ///Is shader custom?
    bool IsCustom(){  
        return m_custom_shader;  
    }

    ///Has shader material alpha channel?
    bool IsAlpha(){  
        return m_is_alpha; 
    }
    ///Has material valid shader?
    bool IsShaderOK(){
        return (m_program > 0);
    }

    ///Get first alpha texture ID
    GLuint GetAlphaTexID(){
        for(m_it_textures = m_textures.begin(); m_it_textures != m_textures.end(); ++m_it_textures){
            if(m_it_textures->second->GetType() == ALPHA)
                return m_it_textures->second->GetID();
        }
        return 0;
    }

	void AddFeature( ShaderFeature* _pFeat );
};

typedef TMaterial* TMaterialPtr;

#endif
