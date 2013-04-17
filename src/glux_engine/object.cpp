/**
****************************************************************************************************
****************************************************************************************************
@file: object.cpp
@brief objects classes, initialization and settings
****************************************************************************************************
***************************************************************************************************/
#include "object.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// TObject methods ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
****************************************************************************************************
@brief Set default values, primitive type and resets position
****************************************************************************************************/
TObject::TObject()
{
    m_type = PRIMITIVE;
    //reset transformation
    m_pos = glm::vec3(0.0);
    m_rot = glm::vec3(0.0);
    m_scale = glm::vec3(1.0);
    m_transform = glm::mat4(1.0);   //initialize transformation matrix

    m_shadow_cast = true;
    m_shadow_receive = true;
    m_draw_object = true;

    //drawmode
    m_instances = 1;
    m_drawmode = GL_TRIANGLES;
    m_vbo.indices = 0;
    m_vbo.vao = 0;
    m_element_indices = false;

    //ID's
    m_sceneID = 0;
    m_matID = 0;
}


/**
****************************************************************************************************
@brief Destructor
@todo free dynamic data
****************************************************************************************************/
TObject::~TObject()
{
    ///delete VBO buffers (only if object is not used in future)
    if(m_vbo.vao != 0)
    {
        glDeleteVertexArrays(1, &m_vbo.vao);
        glDeleteBuffers(4, m_vbo.buffer);
    }

}



/**
****************************************************************************************************
@brief Creates object using direct parameters. Objects are generated directly and then stored in VBO
for faster further drawing.
@param _name object name(must be unique)
@param primitive object type (can be CUBE,PLANE,CONE,CYLINDER,DISK,SPHERE,TORUS,TEAPOT,SPLINE,FONT)
@param size basic object size (depends on primitive type)
@param height object height (depends on primitive type)
@param sliceX polygon count horizontal (depends on primitive type)
@param sliceY polygon count vertical (depends on primitive type)
****************************************************************************************************/
void TObject::Create(const char* name, int primitive, GLfloat size, GLfloat height, GLint sliceX, GLint sliceY)
{
    m_name = name;
    m_scale = glm::vec3(1.0);
    m_shadow_cast = true;
    m_shadow_receive = true;
    m_draw_object = true;
    m_element_indices = true;
    m_instances = 1;

    //object type
    m_type = PRIMITIVE;

    GLfloat x = size/2;
    GLfloat z = height/2;

    //prepare data buffers for object
    vector<GLfloat> vertices, normals,texcoords;
    vector<GLuint> faces;

    ///Every primitive type use parameters differently:
    GLuint verts = 0; //number of vertices
    switch(primitive)
    {
        ///cube: 1 param: size
    case CUBE:
        {
            verts = 20;
            m_vbo.indices = 36;
            m_drawmode = GL_TRIANGLES;

            GLfloat vertices_f[] = {   -size,-size,-size, -size,-size,-size, -size,-size,-size, //0
                                       -size,-size, size, -size,-size, size, -size,-size,size,	//3
                                        size,-size,-size,  size,-size,-size,  size,-size,-size,	//6
                                        size,-size, size,  size,-size, size,  size,-size,size,	//9
                                        size, size,-size,  size, size,-size,                    //12
                                        size, size, size,  size, size, size,				    //14
                                       -size, size,-size, -size, size,-size,		            //16
                                       -size, size, size, -size, size, size				        //18
            };

            GLfloat texcoords_f[] = {   0.0,1.0, 1.0,0.0, 0.0,0.0, 
                                        0.0,0.0, 1.0,1.0, 0.0,1.0, 
                                        1.0,1.0, 0.0,0.0, 1.0,0.0,
                                        1.0,0.0, 0.0,1.0, 1.0,1.0,
                                        1.0,0.0, 0.0,0.0,
                                        1.0,1.0, 0.0,1.0,
                                        0.0,0.0, 1.0,0.0,
                                        0.0,1.0, 1.0,1.0 };

            GLfloat normals_f[] = {    -1.0,-1.0,-1.0, -1.0,-1.0,-1.0, -1.0,-1.0,-1.0,
                                        1.0,-1.0,1.0, 1.0,-1.0,1.0, 1.0,-1.0,1.0,
                                        1.0,-1.0,-1.0, 1.0,-1.0,-1.0, 1.0,-1.0,-1.0,
                                        1.0,-1.0,1.0, 1.0,-1.0,1.0, 1.0,-1.0,1.0,
                                        1.0,1.0,-1.0, 1.0,1.0,-1.0,
                                        1.0,1.0,1.0, 1.0,1.0,1.0,
                                       -1.0,1.0,-1.0, -1.0,1.0,-1.0,
                                       -1.0,1.0,1.0, -1.0,1.0,1.0 };

            GLuint faces_f[] = { 11,5,2,  8,11,2,  14,10,7, 12,14,7, 19,15,13, 17,19,13, 4,18,16, 1,4,16,14,18,3,  3,9,14,  6,0,16,  12,6,16 };

            vertices = vector<GLfloat>(vertices_f, vertices_f + verts*3);
            texcoords = vector<GLfloat>(texcoords_f, texcoords_f + verts*2);
            normals = vector<GLfloat>(normals_f, normals_f + verts*3);
            faces = vector<GLuint>(faces_f, faces_f + m_vbo.indices);
        }
        break;
        ///plane: 2 params, size, sizeY
    case PLANE:
        {
            verts = 4;
            m_vbo.indices = 4;
            m_drawmode = GL_TRIANGLE_STRIP;

            GLfloat vertices_f[] = { -x,0.0,z,   x,0.0,z,  -x,0.0,-z,  x,0.0,-z };
            GLfloat texcoords_f[] = { 0.0,0.0,  1.0,0.0,   0.0,1.0,  1.0,1.0 };
            GLfloat normals_f[] = { 0.0,1.0,0.0,  0.0,1.0,0.0,  0.0,1.0,0.0,  0.0,1.0,0.0 };
            GLuint faces_f[] = { 0,1,2,3 };

            vertices = vector<GLfloat>(vertices_f, vertices_f + 12);
            texcoords = vector<GLfloat>(texcoords_f, texcoords_f + 8);
            normals = vector<GLfloat>(normals_f, normals_f + 12);
            faces = vector<GLuint>(faces_f, faces_f + 4);
        }
        break;
        ///strip plane: 4 params, size, sizeY, sliceX, sliceY
    case STRIP_PLANE:
        {
            GLfloat dx = (GLfloat)size/sliceX;
            GLfloat dy = (GLfloat)height/sliceY;
            GLfloat dtx = 1.0f/sliceX;
            GLfloat dty = 1.0f/sliceY;

            //create vertices, normals and texcoords
            for(int i=0; i<sliceY; i++)
            {
                for(int j=0; j<sliceX; j++)
                {
                    vertices.push_back(j*dx - size/2.0f); vertices.push_back(0.0f); vertices.push_back(i*dy - height/2.0f);
                    normals.push_back(0.0); normals.push_back(1.0); normals.push_back(0.0); 
                    texcoords.push_back(j*dtx); texcoords.push_back(i*dty); 
                }
            }
            //create indices
            for(int i=0; i<sliceY - 2; i++)
            {
                for(int j=0; j<sliceX - 2; j++)
                {
                    int shift = i*sliceX;
                    faces.push_back(shift + j); faces.push_back(shift + j + sliceX); faces.push_back(shift + j + sliceX + 1);
                    faces.push_back(shift + j); faces.push_back(shift + j + sliceX + 1); faces.push_back(shift + j + 1);
                }
            }
            verts = sliceX * sliceY;
            m_vbo.indices = faces.size();
            m_drawmode = GL_TRIANGLES;
        }
        break;
        ///cone: 4 params: baseRadius, height, sliceX, sliceY
    case CONE:
        return;
        ///cylinder: 4 params: baseRadius, height, sliceX, sliceY
    case CYLINDER:

        return;
        ///disk: 4 params: innerRadius, outerRadius, sliceX, sliceY
    case DISK:
        return;
        ///sphere: 3 params: radius, sliceX, sliceY
    case SPHERE:
        {
            GLint rings = sliceX;
            GLint segments = sliceY;

            // set vertex count and index count 
            verts = (rings + 1)*(segments + 1);
            m_vbo.indices = 2 * rings * (segments + 1);
            m_drawmode = GL_TRIANGLE_STRIP;

            /////////////////////////////////////////////////////////////////////////////////////////////////
            //Generate sphere
            GLfloat fDeltaRingAngle = ( PI / rings );
            GLfloat fDeltaSegAngle = ( 2.0f * PI / segments );

            GLuint wVerticeIndex = 0 ;
            // Generate the group of rings for the sphere
            for( int ring = 0; ring < rings + 1 ; ring++ )
            {
                GLfloat r0 = sin( ring * fDeltaRingAngle );
                GLfloat y0 = cos( ring * fDeltaRingAngle );

                // Generate the group of segments for the current ring
                for( int seg = 0; seg < segments + 1 ; seg++ )
                {
                    GLfloat x0 = r0 * sin( seg * fDeltaSegAngle );
                    GLfloat z0 = r0 * cos( seg * fDeltaSegAngle );

                    vertices.push_back(x0*size); vertices.push_back(y0*size); vertices.push_back(z0*size);
                    normals.push_back(x0); normals.push_back(y0); normals.push_back(z0);
                    texcoords.push_back((GLfloat)seg/segments); texcoords.push_back((GLfloat)ring/rings);

                    // add two indices except for last ring 
                    if ( ring != rings ) 
                    {
                        faces.push_back(wVerticeIndex);
                        faces.push_back(wVerticeIndex + (GLuint)(segments + 1));
                        wVerticeIndex ++ ; 
                    } 
                }
            }
        }
        break;
        ///torus: 4 params: innerRadius, outerRadius, nsides, rings
    case TORUS:
        {
            ///(torus creation - thanks to http://www.sulaco.co.za/)
            GLfloat theta, phi, theta1,
                cosTheta, sinTheta,
                cosTheta1, sinTheta1,
                ringDelta, sideDelta,
                cosPhi, sinPhi, dist;
            m_vbo.indices = 0;

            sideDelta = 2.0f * PI / sliceX;
            ringDelta = 2.0f * PI / sliceY;

            theta = 0.0;
            cosTheta = 1.0;
            sinTheta = 0.0;

            for(int i = sliceY - 1; i >= 0; i--)
            {
                theta1 = theta + ringDelta;
                cosTheta1 = cos(theta1);
                sinTheta1 = sin(theta1);
                phi = 0.0;
                for(int j = sliceX; j >= 0; j--)
                {
                    phi = phi + sideDelta;
                    cosPhi = cos(phi);
                    sinPhi = sin(phi);
                    dist = height + (size * cosPhi);

                    texcoords.push_back(cosTheta1 * cosPhi); texcoords.push_back(-sinTheta1 * cosPhi);
                    normals.push_back(cosTheta1 * cosPhi); normals.push_back(-sinTheta1 * cosPhi); normals.push_back(sinPhi); 
                    vertices.push_back(cosTheta1 * dist); vertices.push_back(-sinTheta1 * dist); vertices.push_back(size * sinPhi); 
                    faces.push_back(m_vbo.indices++);

                    texcoords.push_back(cosTheta * cosPhi); texcoords.push_back(-sinTheta * cosPhi);
                    normals.push_back(cosTheta * cosPhi); normals.push_back(-sinTheta * cosPhi); normals.push_back(sinPhi); 
                    vertices.push_back(cosTheta * dist); vertices.push_back(-sinTheta * dist); vertices.push_back(size * sinPhi); 
                    faces.push_back(m_vbo.indices++);
                }
                theta = theta1;
                cosTheta = cosTheta1;
                sinTheta = sinTheta1;
            }

            verts = m_vbo.indices;
            m_drawmode = GL_TRIANGLE_STRIP;
        }
        break;  
    default:
        break;
    }
    //create vertex buffer with data
    glGenVertexArrays(1, &m_vbo.vao);
    glBindVertexArray(m_vbo.vao);

    //Allocate and assign three VBO to our handle
    glGenBuffers(4, m_vbo.buffer);

    //Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) and copy buffer data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, 3 * verts * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);  
    // vertices are on index 0 and contains three floats per vertex
    glVertexAttribPointer(GLuint(0), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //store normals
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_NORMAL]);    
    glBufferData(GL_ARRAY_BUFFER, 3 * verts * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
    // normals are on index 1 and contains three floats per vertex
    glVertexAttribPointer(GLuint(1), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //store texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_TEXCOORD]);    
    glBufferData(GL_ARRAY_BUFFER, 2 * verts * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
    //coordinates are on index 2 and contains two floats per vertex
    glVertexAttribPointer(GLuint(2), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    //store vertex array indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo.buffer[P_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vbo.indices * sizeof(GLuint), &faces[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

/**
****************************************************************************************************
@brief create object as instance from existing object
****************************************************************************************************/
void TObject::CreateInstance(const TObject &ref)
{
    *this = ref;
    m_type = INSTANCE;
}


/**
****************************************************************************************************
@brief Draws object by calling it's display list. Before this object is transformed and rotated to
it's current position. Then attached material is rendered(GeometryMaterial::RenderMaterial())
There are specific types of settings, when object is not rendered (e.g. when rendering into shadow map
is active and object don't cast shadows)
@param tessellate draw object with HW tessellation enabled?
****************************************************************************************************/
void TObject::Draw(bool tessellate)
{
    //don't draw object with draw_object flag set to false
    if(!m_draw_object)
        return;
#ifdef VERBOSE 
    cout<<"Drawing "<<m_name << endl;
#endif

    if(m_type != INSTANCE)
        glBindVertexArray(m_vbo.vao);

    //set patch parameter
    int patch = m_drawmode;
    if(tessellate)
    {
        if(m_drawmode == GL_TRIANGLE_STRIP)
            glPatchParameteri(GL_PATCH_VERTICES, 4);
        else
            glPatchParameteri(GL_PATCH_VERTICES, 3);
         patch = GL_PATCHES;
    }      

    //different drawing mode: simple vertex array or array with element indices
    if(m_element_indices) 
    {
        //it's possible to use geometry instancing (if there is more than 1 object instance)
        if(m_instances > 1)
            glDrawElementsInstanced(patch, m_vbo.indices, GL_UNSIGNED_INT , 0, m_instances);
        else
            glDrawElements(patch, m_vbo.indices, GL_UNSIGNED_INT , 0);
    }
    else 
    {
        if(m_instances > 1)
            glDrawArraysInstanced(patch, 0, m_vbo.indices * 3, m_instances);
        else
            glDrawArrays(patch, 0, m_vbo.indices * 3);
    }
#ifdef VERBOSE 
    cout<<"...done\n";
#endif
}


////////////////////////////////////////////////////////////////////////////////
/////////////////////////// OBJECT TRANSFORMATIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
****************************************************************************************************
@brief Move object from current position by position vector(relative)
@param wx X-coordinate of vector
@param wy Y-coordinate of vector
@param wz Z-coordinate of vector
****************************************************************************************************/
void TObject::Move(GLfloat wx, GLfloat wy, GLfloat wz)
{
    m_pos.x += wx;
    m_pos.y += wy;
    m_pos.z += wz;

    //update matrix
    m_transform = glm::translate(m_transform, glm::vec3(wx,wy,wz));
}

/**
****************************************************************************************************
@brief Move object to new position(relative)
@param wx X-coordinate of vector
@param wy Y-coordinate of vector
@param wz Z-coordinate of vector
****************************************************************************************************/
void TObject::MoveAbs(GLfloat wx, GLfloat wy, GLfloat wz)
{
    m_pos.x = wx;
    m_pos.y = wy;
    m_pos.z = wz;

    //update matrix
    m_transform = glm::mat4(1.0);
    m_transform = glm::translate(m_transform, m_pos);
    m_transform = glm::rotate(m_transform, m_rot.x, glm::vec3(1.0,0.0,0.0) );
    m_transform = glm::rotate(m_transform, m_rot.y, glm::vec3(0.0,1.0,0.0) );
    m_transform = glm::rotate(m_transform, m_rot.z, glm::vec3(0.0,0.0,1.0) );
    m_transform = glm::scale(m_transform, m_scale);
}

/**
****************************************************************************************************
@brief Rotate object by adding angle to current rotation(relative)
@param angle rotation increment in degrees
@param axis rotational axis (can be A_X, A_Y, A_Z)
****************************************************************************************************/
void TObject::Rotate(GLfloat angle, GLint axis)
{
    switch(axis)
    {
    case A_X: 
        m_rot.x += angle; 
        m_transform = glm::rotate(m_transform, angle, glm::vec3(1.0,0.0,0.0) );
        break;
    case A_Y: 
        m_rot.y += angle; 
        m_transform = glm::rotate(m_transform, angle, glm::vec3(0.0,1.0,0.0) );
        break;
    case A_Z: 
        m_rot.z += angle; 
        m_transform = glm::rotate(m_transform, angle, glm::vec3(0.0,0.0,1.0) );
        break;
    default: break;
    }
}

/**
****************************************************************************************************
@brief Rotate object by new angle(absolute)
@param angle new rotation angle in degrees
@param axis rotational axis (can be A_X, A_Y, A_Z)
****************************************************************************************************/
void TObject::RotateAbs(GLfloat angle, GLint axis)
{
    switch(axis)
    {
    case A_X: 
        m_rot.x = angle;
        break;
    case A_Y: 
        m_rot.y = angle;
        break;
    case A_Z: 
        m_rot.z = angle;
        break;
    default:
        break;
    }
    //update matrix
    m_transform = glm::mat4(1.0);
    m_transform = glm::translate(m_transform, m_pos);
    m_transform = glm::rotate(m_transform, m_rot.x, glm::vec3(1.0,0.0,0.0) );
    m_transform = glm::rotate(m_transform, m_rot.y, glm::vec3(0.0,1.0,0.0) );
    m_transform = glm::rotate(m_transform, m_rot.z, glm::vec3(0.0,0.0,1.0) );
    m_transform = glm::scale(m_transform, m_scale);
}

/**
****************************************************************************************************
@brief Resize object
@param sx scale in X-axis (<1.0 - smaller, >1.0 - bigger)
@param sy scale in Y-axis (<1.0 - smaller, >1.0 - bigger)
@param sz scale in Z-axis (<1.0 - smaller, >1.0 - bigger)
****************************************************************************************************/
void TObject::Resize(GLfloat sx, GLfloat sy, GLfloat sz)
{
    m_scale.x = sx;
    m_scale.y = sy;
    m_scale.z = sz;

    //update matrix
    m_transform = glm::scale(m_transform, m_scale);
}
