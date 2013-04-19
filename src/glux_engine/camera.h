/**
****************************************************************************************************
****************************************************************************************************
@file: camera.h
@brief initialization and manipulating with camera - declarations
****************************************************************************************************
***************************************************************************************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "globals.h"

/**
@class TCamera 
@brief scene camera class. Sets default scene camera, performs basic camera movements, pointing to object etc...
***************************************************************************************************/
class TCamera
{
//-----------------------------------------------------------------------------
// -- Member variables

protected:
    ///camera position
    glm::vec3 m_pos;
    ///camera rotation
    glm::vec3 m_rot;
    ///camera look
    glm::vec3 m_look;
    ///camera view matrix
    glm::mat4 m_viewMatrix;
    ///camera type
    int m_type;
	///clipping planes
    float m_near_plane, m_far_plane;
    ///field of view
    float m_fovy;

//-----------------------------------------------------------------------------
//-- Public methods 
public:
    TCamera();
    ~TCamera(){}

    //move camera
    glm::mat4& Move(GLfloat wx, GLfloat wy, GLfloat wz);
    glm::mat4& MoveAbs(GLfloat wx, GLfloat wy, GLfloat wz);
    //rotate camera
    glm::mat4& Rotate(GLfloat angle, GLint axis);
    glm::mat4& RotateAbs(GLfloat angle, GLint axis);
    //camera look
    glm::mat4& LookAt(GLfloat wx, GLfloat wy, GLfloat wz);
    //update camera matrix
    void UpdateMatrix();
	///@brief Reset camera position
    void Reset(){ 
        m_pos = glm::vec3(0.0); 
        m_rot = glm::vec3(0.0); 
        m_look = glm::vec3(0.0); 
    }
	
    //Load & save camera from file
    glm::mat4& Load();
    void Save();
//-----------------------------------------------------------------------------
	//-- Set/Get FOVy
	void SetFOVy( float _fovy ){ m_fovy = _fovy; }
	float GetFOVy(){ return m_fovy; }
	//-- Set/Get near plane
	void SetNearPlane( float _near ){ m_near_plane = _near; }
	float GetNearPlane(){ return m_near_plane; }
	//-- Set/Get far plane
	void SetFarPlane( float _far ){ m_far_plane = _far; }
	float GetFarPlane(){ return m_far_plane; }
    ///@brief get camera position
    glm::vec3& GetPos(){ return m_pos; }
    ///@brief get camera rotation
    glm::vec3& GetRot(){ return m_rot; }
    ///@brief get camera look
    glm::vec3& GetLook(){ return m_look; }
    //-- Set/Get camera view matrix
    void SetMatrix(glm::mat4 &matrix){ m_viewMatrix = matrix; }
    glm::mat4& GetMatrix(){ return m_viewMatrix; }
    //-- Set/Get camera type
	void SetType(int type){ m_type = type; }
	int GetType(){ return m_type; }
};

#endif
