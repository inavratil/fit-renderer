/**
****************************************************************************************************
****************************************************************************************************
@file: camera.cpp
@brief initialization and manipulating with camera
****************************************************************************************************
***************************************************************************************************/
#include "camera.h"

/**
****************************************************************************************************
@brief Initialize camera, sets all vectors to identity
***************************************************************************************************/
TFreelookCamera::TFreelookCamera()
{
    m_pos = glm::vec3(0.0);
    m_look = glm::vec3(0.0);
	m_up = glm::vec3(0.0);

	pitch = 0.0f;
	yaw = 0.0f;
	flags = 0;
	movementSpeed = 50.0f;

	dU = glm::vec3(0.0f, 1.0f, 0.0f);
	dL = glm::vec3(0.0f, 0.0f, -1.0f);

	m_up = glm::vec3(0.0);
	m_right = glm::vec3(0.0);
}


/**
****************************************************************************************************
@brief Update camera matrix by translating and rotating viewport
****************************************************************************************************/
glm::mat4 TFreelookCamera::UpdateMatrix()
{
	float t = (float) timer.GetElapsedTimeSeconds();

	//we will use quaternions to rotate
	glm::quat q = glm::quat(1, 0, 0, 0);
	q = glm::rotate(q, -yaw, glm::vec3(0, 1, 0));
	q = glm::rotate(q, -pitch, m_right);

	glm::mat3 m = glm::mat3_cast(q);

	m_up = m * m_up;
	m_look = m * m_look;
	m_right = glm::cross(m_look, m_up);

	pitch = 0.0f;
	yaw = 0.0f;

	if( (flags & 0x80000000) == 0x80000000 ) m_pos += movementSpeed * t * m_look;	//forward
	if( (flags & 0x40000000) == 0x40000000 ) m_pos -= movementSpeed * t * m_look;	//back
	if( (flags & 0x20000000) == 0x20000000 ) m_pos -= movementSpeed * t * m_right;	//left
	if( (flags & 0x10000000) == 0x10000000 ) m_pos += movementSpeed * t * m_right;	//right

	m_viewMatrix = glm::lookAt(m_pos, m_look+m_pos, m_up);

	timer.Reset();

	return m_viewMatrix;
}

/**
****************************************************************************************************
@brief Save camera into file
****************************************************************************************************/
void TFreelookCamera::Save()
{
    ofstream fout("camera.ini");
    if(!fout){ cerr<<"Can't save camera position"; return; }
    fout<<"POS: "<<m_pos.x<<","<<m_pos.y<<","<<m_pos.z<<"\n";
        //<<"ROT: "<<m_rot.x<<","<<m_rot.y<<","<<m_rot.z<<"\n";
    fout.close();
}

/**
****************************************************************************************************
@brief Load camera from file and 
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TFreelookCamera::Load()
{
    /*
	ifstream fin("camera.ini");
    if(!fin){ cerr<<"Can't load camera position"; return m_viewMatrix; }
    string line; getline(fin, line);
    sscanf(line.c_str(), "POS: %f,%f,%f", &m_pos.x, &m_pos.y, &m_pos.z);
    getline(fin, line);
    sscanf(line.c_str(), "ROT: %f,%f,%f", &m_rot.x, &m_rot.y, &m_rot.z);
    fin.close();
    UpdateMatrix();
	*/
    return m_viewMatrix;
}

void TFreelookCamera::setFreelookCamera(glm::vec3 vPos, glm::vec3 vUp, glm::vec3 vFocusPoint)
{
	m_pos = vPos;
	m_up = vUp;

	m_look = vFocusPoint - vPos;
	m_look = glm::normalize(m_look);

	m_right = glm::cross(m_look, m_up);
	m_up = glm::normalize(glm::cross(m_right, m_look));


	//Its necessary to calculate pitch and yaw agains dL and dU
	//Project dV on XZ plane



	//now we have 3 necessary vectors set (look, up, focus point), set view matrix
	m_viewMatrix = glm::lookAt(m_pos, m_look, m_up);
}

void TFreelookCamera::handleInputMessage(cam_events e)
{
	switch(e)
	{
		case CAMERA_FORWARD_DOWN:
			SetBit(flags, 0);
			break;
		case CAMERA_FORWARD_UP:
			ClearBit(flags, 0);
			break;
		case CAMERA_BACKWARD_DOWN:
			SetBit(flags, 1);
			break;
		case CAMERA_BACKWARD_UP:
			ClearBit(flags, 1);
			break;
		case CAMERA_LEFT_DOWN:
			SetBit(flags, 2);
			break;
		case CAMERA_LEFT_UP:
			ClearBit(flags, 2);
			break;
		case CAMERA_RIGHT_DOWN:
			SetBit(flags, 3);
			break;
		case CAMERA_RIGHT_UP:
			ClearBit(flags, 3);
			break;
		default:
			break;
	}
}

void TFreelookCamera::adjustOrientation(float p, float y)
{
	pitch += p;
	yaw += y;

	//Clamping to <0 ; 360>
	if(pitch > 360.0f)
		pitch -= 360.0f;
	else if(pitch < 0.0f)
		pitch += 360.0f;

	if(yaw > 360.0f)
		yaw -= 360.0f;
	else if(yaw < 0.0f)
		yaw += 360.0f;
}