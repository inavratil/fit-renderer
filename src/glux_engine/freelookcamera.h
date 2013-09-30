/**
****************************************************************************************************
****************************************************************************************************
@file: camera.h
@brief initialization and manipulating with camera - declarations
****************************************************************************************************
***************************************************************************************************/
#ifndef _FREELOOKCAMERA_H_
#define _FREELOOKCAMERA_H_

#include "globals.h"
#include "hires_timer.h"
#include "bitops.h"

/**
@class TFreelookCamera 
@brief scene camera class. Sets default scene camera, performs basic camera movements, pointing to object etc...
***************************************************************************************************/
class TFreelookCamera
{
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

public:
	enum cam_events{CAMERA_FORWARD_DOWN=0, 
					CAMERA_BACKWARD_DOWN, 
					CAMERA_LEFT_DOWN, 
					CAMERA_RIGHT_DOWN, 
					CAMERA_FORWARD_UP, 
					CAMERA_BACKWARD_UP, 
					CAMERA_LEFT_UP,
					CAMERA_RIGHT_UP};

    TFreelookCamera();
    ~TFreelookCamera(){}
    
    //update camera matrix
    glm::mat4 UpdateMatrix();

	void setFreelookCamera(glm::vec3 vPos, glm::vec3 vUp, glm::vec3 vLookAt);
	void handleInputMessage(cam_events e);
	void adjustOrientation(float pitch, float yaw);

    ///@brief get camera position
    glm::vec3& GetPos(){ 
        return m_pos; 
    }

	///@brief get camera look
    glm::vec3& GetLook(){ 
        return m_look; 
    }

	///@brief get camera up
    glm::vec3& GetUp(){ 
        return m_up; 
    }

	void setMovementSpeed(float speed)
	{
		if(speed<=0.0f)
			return;

		movementSpeed = speed;
	}

	//TODO: PREPISAT!
    ///@brief Reset camera position
   // void Reset(){ 
     //   m_pos = glm::vec3(0.0); 
       // m_look = glm::vec3(0.0); 
    //}

    ///@brief Get camera view matrix
    glm::mat4& GetMatrix(){
        return m_viewMatrix;
    }

    //Load & save camera from file
    glm::mat4& Load();
    void Save();

};

#endif
