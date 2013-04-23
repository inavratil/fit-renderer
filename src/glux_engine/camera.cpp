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
TCamera::TCamera() :
	m_near_plane( 0 ),
	m_far_plane( 0 ),
	m_fovy( 0 )
{
    m_pos = glm::vec3(0.0);
    m_rot = glm::vec3(0.0);
    m_look = glm::vec3(0.0);
    m_type = FPS;
}

/**
****************************************************************************************************
@brief Rotate camera around axis(relative)
@param angle rotation angle in degrees
@param axis rotation axis (can be A_X, A_Y, A_Z)
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TCamera::Rotate(GLfloat angle, GLint axis)
{
    switch(axis)
    {
    case A_X: m_rot.x += angle;
        break;
    case A_Y: m_rot.y += angle;
        break;
    case A_Z: m_rot.z += angle;
        break;
    default:
        break;
    }
    UpdateMatrix();
    return m_viewMatrix;
}

/**
****************************************************************************************************
@brief Rotate camera around axis(absolute)
@param angle rotation angle in degrees
@param axis rotation axis (can be A_X, A_Y, A_Z)
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TCamera::RotateAbs(GLfloat angle, GLint axis)
{
    switch(axis)
    {
    case A_X: m_rot.x = angle;
        break;
    case A_Y: m_rot.y = angle;
        break;
    case A_Z: m_rot.z = angle;
        break;
    default:
        break;
    }
    UpdateMatrix();
    return m_viewMatrix;
}

/**
****************************************************************************************************
@brief Move camera to new position(relative)
@param wx new X-coordinate
@param wy new Y-coordinate
@param wz new Z-coordinate
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TCamera::Move(GLfloat wx, GLfloat wy, GLfloat wz)
{
    m_pos.x += wx;
    m_pos.y += wy;
    m_pos.z += wz;
    UpdateMatrix();
    return m_viewMatrix;
}

/**
****************************************************************************************************
@brief Move camera to new position(absolute)
@param wx new X-coordinate
@param wy new Y-coordinate
@param wz new Z-coordinate
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TCamera::MoveAbs(GLfloat wx, GLfloat wy, GLfloat wz)
{
    m_pos.x = wx;
    m_pos.y = wy;
    m_pos.z = wz;
    UpdateMatrix();
    return m_viewMatrix;
}

/**
****************************************************************************************************
@brief Look camera at target.
@param wx target X-coordinate
@param wy target Y-coordinate
@param wz target Z-coordinate
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TCamera::LookAt(GLfloat wx, GLfloat wy, GLfloat wz)
{
    m_look.x = wx;
    m_look.y = wy;
    m_look.z = wz;
    UpdateMatrix();
    return m_viewMatrix;
}


/**
****************************************************************************************************
@brief Update camera matrix by translating and rotating viewport
****************************************************************************************************/
void TCamera::UpdateMatrix()
{
    
    m_viewMatrix = glm::mat4(1.0);
    //move camera absolute or
    if(m_look == glm::vec3(0.0) )
    {
        if(m_type == ORBIT)
            m_viewMatrix = glm::translate(m_viewMatrix, m_pos);
        m_viewMatrix = glm::rotate(m_viewMatrix, m_rot.x, glm::vec3(1.0,0.0,0.0) );
        m_viewMatrix = glm::rotate(m_viewMatrix, m_rot.y, glm::vec3(0.0,1.0,0.0) );
        if(m_type == FPS)
            m_viewMatrix = glm::translate(m_viewMatrix, m_pos);

    }
    //by looking position
    else
    {
        m_viewMatrix = glm::lookAt(m_pos, m_look, glm::vec3(0.0,1.0,0.0));
    }
}

/**
****************************************************************************************************
@brief Save camera into file
****************************************************************************************************/
void TCamera::Save()
{
    ofstream fout("camera.ini");
    if(!fout){ cerr<<"Can't save camera position"; return; }
    fout<<"POS: "<<m_pos.x<<","<<m_pos.y<<","<<m_pos.z<<"\n"
        <<"ROT: "<<m_rot.x<<","<<m_rot.y<<","<<m_rot.z<<"\n";
    fout.close();
}

/**
****************************************************************************************************
@brief Load camera from file and 
@return new transformation matrix
****************************************************************************************************/
glm::mat4& TCamera::Load()
{
    ifstream fin("camera.ini");
    if(!fin){ cerr<<"Can't load camera position"; return m_viewMatrix; }
    string line; getline(fin, line);
    sscanf(line.c_str(), "POS: %f,%f,%f", &m_pos.x, &m_pos.y, &m_pos.z);
    getline(fin, line);
    sscanf(line.c_str(), "ROT: %f,%f,%f", &m_rot.x, &m_rot.y, &m_rot.z);
    fin.close();
    UpdateMatrix();
    return m_viewMatrix;
}
