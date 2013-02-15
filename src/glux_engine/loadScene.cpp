/**
****************************************************************************************************
****************************************************************************************************
@file: loadScene.cpp
@brief loads entire scene from 3DS file using lib3ds: http://code.google.com/p/lib3ds/
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"

/**
****************************************************************************************************
@brief Loads entire scene from file using Assimp
@param file file with scene, almost any format recognized by Assimp
@param load_materials shall we load materials?
@param load_lights shall we load lights? Currently ignored
@param name_space prefix in object name (when using same model scene in different scenes)
****************************************************************************************************/
void TScene::LoadScene(const char* file, bool load_materials, bool load_lights, string name_space)
{
	Assimp::Importer importer;

	//Do not import line and point meshes
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

	const aiScene* scene = importer.ReadFile(file,	aiProcess_JoinIdenticalVertices|
		aiProcess_LimitBoneWeights|
		aiProcess_RemoveRedundantMaterials|
		aiProcess_PreTransformVertices |
		aiProcess_Triangulate|
		aiProcess_GenUVCoords|
		aiProcess_SortByPType|
		aiProcess_FindDegenerates|
		aiProcess_FindInvalidData|
		aiProcess_FixInfacingNormals|
		aiProcess_GenNormals);

	if(!scene)
	{
		ShowMessage("Cannot open file with scene!\n",false);
        throw ERR;
	}

	//Load materials
	vector<string> mats;						//array with materials (to index them)
    if(load_materials)
    {
		cout<<scene->mNumMaterials<<" materials. Loading textures:\n";
		for(unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			//materials
			aiMaterial *m = scene->mMaterials[i];

			//get material name
			aiString name; 
			m->Get(AI_MATKEY_NAME,name);

            //remove not-allowed chars from name
            string m_name = name_space + name.C_Str();
            for(unsigned i=0; i<m_name.length(); i++)
            {
                if(m_name[i] < 0 || (!isalpha(m_name[i]) && !isdigit(m_name[i])) || m_name[i] > 128)
					m_name.replace(i--,1,"");
            }
            cout<<"Adding material "<<m_name<<endl;

			//get material properties
			aiColor3D ambient,diffuse,specular;
			float shininess;

			m->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			m->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			m->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			m->Get(AI_MATKEY_SHININESS, shininess);

            AddMaterial(
                m_name.c_str(),									//material name
				glm::vec3(ambient.r, ambient.g, ambient.b),		//ambient color
                glm::vec3(diffuse.r, diffuse.g, diffuse.b),		//diffuse color
                glm::vec3(specular.r, specular.g, specular.b),	//specular color
				256.0f - 256.0f*shininess	
				);
			mats.push_back(m_name);

			//Textures
			string path;
			aiString pth;
			aiReturn texFound;
			unsigned int i=0;

			//Loads up base textures for material
			if(m->GetTextureCount(aiTextureType_DIFFUSE)>0)
			{
				unsigned int numTex = m->GetTextureCount(aiTextureType_DIFFUSE);
				for(i=0; i<numTex; ++i)
				{
					//Get ambient textures
					texFound = m->GetTexture(aiTextureType_DIFFUSE, i, &pth);
					if(texFound==AI_FAILURE)
						break;

					path = "data/tex/";
					path += pth.C_Str();

					AddTexture(m_name.c_str(), path.c_str(), BASE, MODULATE, 1.0f, 1.0f, 1.0f, true, true);
				}
			}

			//bump texture
			if(m->GetTextureCount(aiTextureType_NORMALS)>06)
			{
				unsigned int numTex = m->GetTextureCount(aiTextureType_NORMALS);

				for(i=0; i<numTex; ++i)
				{
					//Get ambient textures
					texFound = m->GetTexture(aiTextureType_NORMALS, i, &pth);
					if(texFound==AI_FAILURE)
						break;

					path = "data/tex/";
					path += pth.C_Str();

					AddTexture(m_name.c_str(), path.c_str(), BUMP, MODULATE, 1.0f, 1.0f, 1.0f, true, true);
				}
			}

			//bump texture
			if(m->GetTextureCount(aiTextureType_HEIGHT)>0)
			{
				unsigned int numTex = m->GetTextureCount(aiTextureType_HEIGHT);

				for(i=0; i<numTex; ++i)
				{
					//Get ambient textures
					texFound = m->GetTexture(aiTextureType_HEIGHT, i, &pth);
					if(texFound==AI_FAILURE)
						break;

					path = "data/tex/";
					path += pth.C_Str();

					AddTexture(m_name.c_str(), path.c_str(), BUMP, MODULATE, 1.0f, 1.0f, 1.0f, true, true);
				}
			}
		}
	}//if(load_materials)

	//update load list
	UpdateLoadList(scene->mNumMeshes + 2*scene->mNumMaterials);

	cout<<"Adding "<<scene->mNumMeshes<<" objects, ";

	//Load meshes
	aiMesh * mesh;
	unsigned int nVertices;
	unsigned int polygons = 0;
	for(unsigned int i=0; i<scene->mNumMeshes; ++i)
	{
		mesh = scene->mMeshes[i];

		if(mesh->mNumFaces==0)
			continue;

		nVertices = mesh->mNumVertices;
		polygons += mesh->mNumFaces;

		//Create object from mesh
		string oname = name_space + mesh->mName.C_Str();

		if(oname.length()==0)
		{
			stringstream ss;
			ss << "FITMesh" << i;
			oname.assign(ss.str().c_str());
		}

		TObject *o = new TObject();
		m_objects[oname] = o;

		m_obj_cache[oname] = m_objects[oname]->Create(mesh);

		//assign material
		if(load_materials)
		{
			SetMaterial(oname.c_str(), mats[mesh->mMaterialIndex].c_str());
		}
		//set sceneID
		m_objects[oname]->SetSceneID(m_sceneID);

		LoadScreen();
	}

	cout<<polygons<<" polygons.\nScene loaded.\n\n";
}
