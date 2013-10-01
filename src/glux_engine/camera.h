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
#include "hires_timer.h"
#include "bitops.h"

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

    ///camera look
    glm::vec3 m_look;

	//camera base vectors
	glm::vec3 dU, dL;

	glm::vec3 m_up;
	glm::vec3 m_right;
	float pitch;
	float yaw;
	float movementSpeed;

	unsigned int flags;
	HRTimer timer;

    ///camera view matrix
    glm::mat4 m_viewMatrix;

	///clipping planes
    float m_near_plane, m_far_plane;
    ///field of view
    float m_fovy;

//-----------------------------------------------------------------------------
//-- Public methods 
public:
	enum cam_events{CAMERA_FORWARD_DOWN=0, 
		CAMERA_BACKWARD_DOWN, 
		CAMERA_LEFT_DOWN, 
		CAMERA_RIGHT_DOWN, 
		CAMERA_FORWARD_UP, 
		CAMERA_BACKWARD_UP, 
		CAMERA_LEFT_UP,
		CAMERA_RIGHT_UP};

    TCamera();
    ~TCamera(){}

    //update camera matrix
    glm::mat4 UpdateMatrix();
	
	void setFreelookCamera(glm::vec3 vPos, glm::vec3 vUp, glm::vec3 vLookAt);
	void handleInputMessage(cam_events e);
	void adjustOrientation(float pitch, float yaw);

	///@brief Reset camera position
    //void Reset(){ 
    //    m_pos = glm::vec3(0.0); 
    //    m_rot = glm::vec3(0.0); 
    //    m_look = glm::vec3(0.0); 
    //}
	
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
    ///@brief get camera look
    glm::vec3& GetLook(){ return m_look; }
	///@brief get camera up
    glm::vec3& GetUp(){ return m_up; }
    //-- Set/Get camera view matrix
    void SetMatrix(glm::mat4 &matrix){ m_viewMatrix = matrix; }
    glm::mat4& GetMatrix(){ return m_viewMatrix; }

	void setMovementSpeed(float speed)
	{
		if(speed<=0.0f)
			return;

		movementSpeed = speed;
	}
};

#endif
