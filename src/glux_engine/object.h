/**
****************************************************************************************************
****************************************************************************************************
@file: object.h
@brief objects classes, initialization and settings
Uses lib3DS to load 3D objects/scenes: http://code.google.com/p/lib3ds/
****************************************************************************************************
***************************************************************************************************/
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "globals.h"

///Object types
enum Obj_types{PRIMITIVE,EXTERN,INSTANCE};
///Basic primitive types
enum Primitives{CUBE,PLANE,STRIP_PLANE,CONE,CYLINDER,DISK,SPHERE,TORUS,SPLINE,FONT};

///vertex buffer indices
enum VBOindices{P_VERTEX,P_NORMAL,P_TEXCOORD,P_INDEX};
///data structure to store vertex/texture/faces data
typedef GLfloat TVertex[3];
///data structure to store texture data
typedef GLfloat TCoord[2];

///@brief Vertex buffer objects structure
struct VBO{
    ///pointers to all buffers: vertex, normal, texcoordinate and faces
    GLuint buffer[4];
    ///pointer vertex array
    GLuint vao;
    ///number of indice
    GLuint indices;
};


/**
@class TObject
@brief contains object information(unique name, type), 3D data(vertices, faces, normal and texture coordinates),
position, rotation and scale, link to material and some specific settings (like shadow casting)
***************************************************************************************************/
class TObject
{
private:
    glm::vec3 m_pos, m_rot, m_scale;    //position, rotation and scale
    bool m_draw_object;   //should be object drawn?

    //shadows settings
    bool m_shadow_cast;
    bool m_shadow_receive;

    //transformation matrix
    glm::mat4 m_transform;

    int m_type;
    string m_name;     //object name
    unsigned m_matID;  //attached material ID
    VBO m_vbo;         //vertex buffer object
    GLenum m_drawmode; //VBO drawing mode
    bool m_element_indices;			//do we  have element indices instead of vertex array?

    //object instances used for geometry instancing
    GLint m_instances;

    //scene ID - when drawing more scenes than 1
    int m_sceneID;

public:

    //constructors
    TObject();
    TObject(const char *name, int primitive, GLfloat size, GLfloat height, GLint sliceX, GLint sliceY){
        m_sceneID = 0; m_matID = 0;        
        Create(name, primitive, size, height, sliceX, sliceY);
    }

    //free dynamic data
    ~TObject();

    //create object as primitive
    void Create(const char *name, int primitive, GLfloat size, GLfloat height, GLint sliceX, GLint sliceY);
    //create object from 3DS file
    VBO Create(const char *name, const char *file, bool load = true);
    //create object from 3DS mesh
    VBO Create(aiMesh *mesh);
    //create object as instance from existing object
    void CreateInstance(const TObject &ref);
    ///@brief Set new VBO data
    ///@param data already loaded VBO data
    void SetVBOdata(VBO &data){ 
        m_vbo = data; 
    }    
    ///@brief Attach material to object
    ///@param _matID material ID
    void SetMaterial(int _matID){ 
        m_matID = _matID; 
    }
    ///@brief Set number of current object GI instances
    void SetGInstances(GLint count){ 
        if(count > 1) m_instances = count; 
    }
    ///@brief Return vertex buffer ID
    GLint GetVertexBuffer(){ 
        return m_vbo.buffer[0]; 
    }


    //draw object (with or without materials)
    void Draw(bool tessellate = false);
    //draw screen aligned quad
    void DrawScreenQuad();
    ///@brief turn on/off object drawing
    void DrawObject(bool flag){ 
        m_draw_object = flag; 
    }

    //object transformation
    void Move(GLfloat wx, GLfloat wy, GLfloat wz);
    void MoveAbs(GLfloat wx, GLfloat wy, GLfloat wz);
    ///@brief Move object to new absolute position
    ///@param w new position coordinates
    void MoveAbs(glm::vec3 w){ 
        m_pos = w; 
    }
    //object rotation
    void Rotate(GLfloat angle, GLint axis);
    void RotateAbs(GLfloat angle, GLint axis);
    //object resize
    void Resize(GLfloat sx, GLfloat sy, GLfloat sz);

    ///@brief Enable/disable shadow casting by object
    void CastShadow(bool flag = true){ 
        m_shadow_cast = flag; 
    }
    ///@brief Does object cast shadow
    bool IsShadow(){ 
        return m_shadow_cast; 
    }

    ///@brief Return object name
    string GetName(){ 
        return m_name; 
    }
    ///@brief Return material ID attached to object
    unsigned GetMatID(){ 
        return m_matID; 
    }
    ///@brief Return object position
    glm::vec3 GetPosition(){ 
        return m_pos; 
    }
    ///@brief Return object transformation matrix
    glm::mat4& GetMatrix(){ 
        return m_transform; 
    }

    ///Get scene ID
    int GetSceneID(){  
        return m_sceneID; 
    }
    ///Set scene ID
    void SetSceneID(int id){  
        m_sceneID = id; 
    }
};
#endif
