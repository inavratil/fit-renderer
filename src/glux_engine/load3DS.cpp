/**
****************************************************************************************************
****************************************************************************************************
@file: load3DS.cpp
@brief loads 3DS object from file
Uses lib3DS to load 3D objects/scenes: http://code.google.com/p/lib3ds/
****************************************************************************************************
***************************************************************************************************/
#include "object.h"


/**
****************************************************************************************************
@brief Creates object directly from 3DS mesh
@param mesh 3DS mesh
@return pointer to vertex buffer with data
****************************************************************************************************/
VBO TObject::Create(Lib3dsMesh *mesh)
{  
    m_name = mesh->name;
    m_scale = glm::vec3(1.0);
    m_shadow_cast = true;
    m_shadow_receive = true;
    m_draw_object = true;
    m_type = EXTERN;
    m_drawmode = GL_TRIANGLES;
    m_element_indices = false;

    //count faces
    m_vbo.indices = mesh->nfaces;

    // Allocate memory for our vertices and normals
    TVertex *vertices = new TVertex[m_vbo.indices * 3];
    TVertex *normals = new TVertex[m_vbo.indices * 3];
    TCoord *texcoords = new TCoord[m_vbo.indices * 3];

    unsigned int FinishedFaces = 0;
    lib3ds_mesh_calculate_vertex_normals(mesh, &normals[FinishedFaces*3]);
    // Loop through every face
    for(unsigned curr_face = 0; curr_face < mesh->nfaces; curr_face++)
    {
        Lib3dsFace *face = &mesh->faces[curr_face];
        for(unsigned i = 0; i < 3; i++)
        {
            //vertices
            memcpy(&vertices[FinishedFaces*3 + i], mesh->vertices[face->index[i]], sizeof(TVertex) );
            //texture coordinates (if present)
            if(mesh->texcos != NULL)
                memcpy(&texcoords[FinishedFaces*3 + i], mesh->texcos[face->index[i]], sizeof(TCoord) );
        }
        FinishedFaces++;
    }
    //swap Y and Z coordinate in normals and vertices (3ds BUG)
    for(unsigned i=0; i<m_vbo.indices * 3; i++)
    {
        std::swap(*(vertices[i]+1),*(vertices[i]+2));
        *(vertices[i]+2) *= -1.0;
        std::swap(*(normals[i]+1),*(normals[i]+2));
        *(normals[i]+2) *= -1.0;
    }

    //Vertex array
    glGenVertexArrays(1, &m_vbo.vao);
    glBindVertexArray(m_vbo.vao);

    //Allocate and assign three VBO to our handle (vertices, normals and texture coordinates)
    glGenBuffers(4, m_vbo.buffer);

    //store vertices into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex) * 3 * m_vbo.indices, vertices, GL_STATIC_DRAW);
    // vertices are on index 0 and contains three floats per vertex
    glVertexAttribPointer(GLuint(0), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //store normals into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex) * 3 * m_vbo.indices, normals, GL_STATIC_DRAW);
    // normals are on index 1 and contains three floats per vertex
    glVertexAttribPointer(GLuint(1), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //store texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_TEXCOORD]);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(TCoord) * 3 * m_vbo.indices, texcoords, GL_STATIC_DRAW);
    //coordinates are on index 2 and contains two floats per vertex
    glVertexAttribPointer(GLuint(2), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Clean up our allocated memory
    delete [] vertices;
    delete [] normals;
    delete [] texcoords;

    //return VBO structure
    return m_vbo;
}



/**
****************************************************************************************************
@brief Creates object from external 3DS file
@param name object name
@param file file with object data in 3DS format
@param load has object data been loaded before?
@return success/fail of loading
****************************************************************************************************/
VBO TObject::Create(const char *name, const char *file, bool load)
{  
    m_name = name;
    m_scale = glm::vec3(1.0);
    m_shadow_cast = true;
    m_shadow_receive = true;
    m_draw_object = true;
    m_type = EXTERN;
    m_drawmode = GL_TRIANGLES;
    m_element_indices = false;

    //don't load object if it has been loaded before
    if(!load) 
    {
        //set VAO and VBO from existing data
        glBindVertexArray(m_vbo.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_VERTEX]);
        // vertices are on index 0 and contains three floats per vertex
        glVertexAttribPointer(GLuint(0), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_NORMAL]);
        // normals are on index 1 and contains three floats per vertex
        glVertexAttribPointer(GLuint(1), 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_TEXCOORD]); 
        //coordinates are on index 2 and contains two floats per vertex
        glVertexAttribPointer(GLuint(2), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo.buffer[P_INDEX]);

        glBindVertexArray(0);
        return m_vbo;
    }

    /////////////////////////////////////////////////////////////////////////////
    //Load 3DS file using lib3ds
    cout<<"Loading object "<<file<<"...";
    Lib3dsFile *model;
    model = lib3ds_file_open(file);
    if(!model)
    {
        ShowMessage("Cannot open 3DS file!",false);
        return VBO();
    }

    //count faces
    m_vbo.indices = 0;
    // Loop through every mesh
    for(int i=0; i < model->nmeshes; i++)
    {
        // Add the number of faces this mesh has to the total faces
        m_vbo.indices += model->meshes[i]->nfaces;
    }

    // Allocate memory for our vertices and normals
    TVertex *vertices = new TVertex[m_vbo.indices * 3];
    TVertex *normals = new TVertex[m_vbo.indices * 3];
    TCoord *texcoords = new TCoord[m_vbo.indices * 3];

    unsigned int FinishedFaces = 0;
    // Loop through all the meshes
    for(int curr_mesh = 0; curr_mesh < model->nmeshes; curr_mesh++)
    {
        Lib3dsMesh *mesh = model->meshes[curr_mesh];
        lib3ds_mesh_calculate_vertex_normals(mesh, &normals[FinishedFaces*3]);
        // Loop through every face
        for(unsigned curr_face = 0; curr_face < mesh->nfaces; curr_face++)
        {
            Lib3dsFace *face = &mesh->faces[curr_face];
            for(unsigned i = 0; i < 3; i++)
            {
                //vertices
                memcpy(&vertices[FinishedFaces*3 + i], mesh->vertices[face->index[i]], sizeof(TVertex) );
                //texture coordinates
                if(mesh->texcos != NULL)
                    memcpy(&texcoords[FinishedFaces*3 + i], mesh->texcos[face->index[i]], sizeof(TCoord) );
            }
            FinishedFaces++;
        }
    }

    //swap Y and Z coordinate in normals and vertices (3ds BUG)
    for(unsigned i=0; i<m_vbo.indices * 3; i++)
    {
        std::swap(*(vertices[i]+1),*(vertices[i]+2));
        *(vertices[i]+2) *= -1.0;
        std::swap(*(normals[i]+1),*(normals[i]+2));
        *(normals[i]+2) *= -1.0;
    }

    //Vertex array
    glGenVertexArrays(1, &m_vbo.vao);
    glBindVertexArray(m_vbo.vao);

    //Allocate and assign three VBO to our handle (vertices, normals and texture coordinates)
    glGenBuffers(4, m_vbo.buffer);

    //store vertices into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex) * 3 * m_vbo.indices, vertices, GL_STATIC_DRAW);
    // vertices are on index 0 and contains three floats per vertex
    glVertexAttribPointer(GLuint(0), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //store normals into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex) * 3 * m_vbo.indices, normals, GL_STATIC_DRAW);
    // normals are on index 1 and contains three floats per vertex
    glVertexAttribPointer(GLuint(1), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //store texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.buffer[P_TEXCOORD]);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(TCoord) * 3 * m_vbo.indices, texcoords, GL_STATIC_DRAW);
    //coordinates are on index 2 and contains two floats per vertex
    glVertexAttribPointer(GLuint(2), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Clean up our allocated memory
    delete [] vertices;
    delete [] normals;
    delete [] texcoords;

    // We no longer need lib3ds
    lib3ds_file_free(model);
    model = NULL;

    cout<<"Done(vertices: "<<m_vbo.indices*3<<", faces: "<<m_vbo.indices<<")\n";

    //return VBO structure
    return m_vbo;
}
