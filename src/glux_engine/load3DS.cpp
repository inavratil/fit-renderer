/**
****************************************************************************************************
****************************************************************************************************
@file: load3DS.cpp
@brief loads 3DS object from file
****************************************************************************************************
***************************************************************************************************/
#include "object.h"


/**
****************************************************************************************************
@brief Creates object directly from aiMesh
@param mesh aiMesh mesh
@return pointer to vertex buffer with data
****************************************************************************************************/
VBO TObject::Create(aiMesh *mesh)
{  
	m_name = mesh->mName.C_Str();
    m_scale = glm::vec3(1.0);
    m_shadow_cast = true;
    m_shadow_receive = true;
    m_draw_object = true;
    m_type = EXTERN;
    m_drawmode = GL_TRIANGLES;
    m_element_indices = false;

    //count faces
	m_vbo.indices = mesh->mNumFaces;

    // Allocate memory for our vertices and normals
    TVertex *vertices = new TVertex[m_vbo.indices * 3];
    TVertex *normals = new TVertex[m_vbo.indices * 3];
    TCoord *texcoords = new TCoord[m_vbo.indices * 3];

    unsigned int FinishedFaces = 0;
    
    // Loop through every face
	for(unsigned curr_face = 0; curr_face < mesh->mNumFaces; curr_face++)
    {
		aiFace *face = &mesh->mFaces[curr_face];

		//aiProcess_Triangulate is turned on, so looping through 3 vertices
        for(unsigned i = 0; i < 3; i++)
        {
			TVertex v;
			v[0] = mesh->mVertices[face->mIndices[i]].x;
			v[1] = mesh->mVertices[face->mIndices[i]].y;
			v[2] = mesh->mVertices[face->mIndices[i]].z;

            //vertices
			memcpy(&vertices[FinishedFaces*3 + i], &v, sizeof(TVertex) );

			//normals
			v[0] = mesh->mNormals[face->mIndices[i]].x;
			v[1] = mesh->mNormals[face->mIndices[i]].y;
			v[2] = mesh->mNormals[face->mIndices[i]].z;

			memcpy(&normals[FinishedFaces*3 + i], &v, sizeof(TVertex) );

            //texture coordinates (if present)
			if(mesh->HasTextureCoords(0))
			{
				TCoord c;
				c[0] = mesh->mTextureCoords[0][face->mIndices[i]].x;
				c[1] = mesh->mTextureCoords[0][face->mIndices[i]].y;
                memcpy(&texcoords[FinishedFaces*3 + i], &c, sizeof(TCoord));
			}
        }
        FinishedFaces++;
    }

	OBB = new BoundingVolume((float*)vertices, 3*mesh->mNumFaces);

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
    //Load 3D Model

	//Do not import line and point meshes
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	const aiScene* model = importer.ReadFile(file,	aiProcessPreset_TargetRealtime_Quality);

	if(!model)
	{
		ShowMessage("Cannot open file with scene!\n",false);
        throw ERR;
	}

    //count faces
    m_vbo.indices = 0;
    // Loop through every mesh
	for(int i=0; i < model->mNumMeshes; i++)
    {
        // Add the number of faces this mesh has to the total faces
		m_vbo.indices += model->mMeshes[i]->mNumFaces;
    }

    // Allocate memory for our vertices and normals
    TVertex *vertices = new TVertex[m_vbo.indices * 3];
    TVertex *normals = new TVertex[m_vbo.indices * 3];
    TCoord *texcoords = new TCoord[m_vbo.indices * 3];

    unsigned int FinishedFaces = 0;
    // Loop through all the meshes
	for(int curr_mesh = 0; curr_mesh < model->mNumMeshes; curr_mesh++)
    {
		aiMesh *mesh = model->mMeshes[curr_mesh];
        //lib3ds_mesh_calculate_vertex_normals(mesh, &normals[FinishedFaces*3]);
        // Loop through every face
		for(unsigned curr_face = 0; curr_face < mesh->mNumFaces; curr_face++)
        {
            aiFace *face = &mesh->mFaces[curr_face];
            for(unsigned i = 0; i < 3; i++)
			{
				TVertex v;
				v[0] = mesh->mVertices[face->mIndices[i]].x;
				v[1] = mesh->mVertices[face->mIndices[i]].y;
				v[2] = mesh->mVertices[face->mIndices[i]].z;

				//vertices
				memcpy(&vertices[FinishedFaces*3 + i], &v, sizeof(TVertex) );

				//normals
				v[0] = mesh->mNormals[face->mIndices[i]].x;
				v[1] = mesh->mNormals[face->mIndices[i]].y;
				v[2] = mesh->mNormals[face->mIndices[i]].z;

				memcpy(&normals[FinishedFaces*3 + i], &v, sizeof(TVertex) );

				//texture coordinates (if present)
				if(mesh->HasTextureCoords(0))
				{
					TCoord c;
					c[0] = mesh->mTextureCoords[0][face->mIndices[i]].x;
					c[1] = mesh->mTextureCoords[0][face->mIndices[i]].y;
					memcpy(&texcoords[FinishedFaces*3 + i], &c, sizeof(TCoord));
				}
			}
            FinishedFaces++;
        }
    }

	OBB = new BoundingVolume((float*)vertices, 3 * m_vbo.indices);

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
    //lib3ds_file_free(model);
    model = NULL;

    cout<<"Done(vertices: "<<m_vbo.indices*3<<", faces: "<<m_vbo.indices<<")\n";

    //return VBO structure
    return m_vbo;
}