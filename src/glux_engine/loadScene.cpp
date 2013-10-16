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

	const aiScene* scene = importer.ReadFile(file,	
		aiProcess_JoinIdenticalVertices|
		aiProcess_LimitBoneWeights|
		aiProcess_RemoveRedundantMaterials|
		aiProcess_PreTransformVertices |
		aiProcess_Triangulate|
		aiProcess_GenUVCoords|
		aiProcess_SortByPType|
		aiProcess_FindDegenerates|
		aiProcess_FindInvalidData|
		//aiProcess_FixInfacingNormals|
		aiProcess_GenSmoothNormals
		);

	if(!scene)
	{
		ShowMessage("Cannot open file with scene!\n",false);
        throw ERR;
	}

	//Load materials
	vector<int> mats;						//array with materials (to index them)
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
            string mat_name = name_space + name.C_Str();
            for(unsigned j=0; j<mat_name.length(); j++)
            {
                if(mat_name[j] < 0 || (!isalpha(mat_name[j]) && !isdigit(mat_name[j])) || mat_name[j] > 128)
					mat_name.replace(j--,1,"");
            }

			//get material properties
			aiColor3D ambient,diffuse,specular;
			float shininess;

			aiReturn r;
			r = m->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			r = m->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			r = m->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			r = m->Get(AI_MATKEY_SHININESS, shininess);

			GeometryMaterial* mat = new GeometryMaterial( mat_name.c_str() );
			mat->SetColor( 
				glm::vec3(ambient.r, ambient.g, ambient.b), 
				glm::vec3(diffuse.r, diffuse.g, diffuse.b), 
				glm::vec3(specular.r, specular.g, specular.b) 
				);
			mat->SetShininess( 256.0f );
			m_material_manager->AddMaterial( mat );

			mats.push_back( mat->GetID() );

			//Textures
			string path;
			aiString pth;
			aiReturn texFound;

			//Loads up base textures for material
			if(m->GetTextureCount(aiTextureType_DIFFUSE)>0)
			{
				unsigned int numTex = m->GetTextureCount(aiTextureType_DIFFUSE);
				for(int j=0; j<numTex; ++j)
				{
					//Get ambient textures
					texFound = m->GetTexture(aiTextureType_DIFFUSE, j, &pth);
					if(texFound==AI_FAILURE)
						break;

					path = "data/tex/";
					path += pth.C_Str();
					
					TexturePtr tex = m_texture_cache->CreateFromImage( path.c_str() );
					mat->AddTexture( tex );
					//AddTexture(m_name.c_str(), path.c_str(), BASE, MODULATE, 1.0f, 1.0f, 1.0f, true, true);
				}
			}

			//bump texture
			if(m->GetTextureCount(aiTextureType_NORMALS)>06)
			{
				unsigned int numTex = m->GetTextureCount(aiTextureType_NORMALS);

				for(unsigned int j=0; j<numTex; ++j)
				{
					//Get ambient textures
					texFound = m->GetTexture(aiTextureType_NORMALS, j, &pth);
					if(texFound==AI_FAILURE)
						break;

					path = "data/tex/normal/";
					path += pth.C_Str();

					TexturePtr tex = m_texture_cache->CreateFromImage( path.c_str() );
					tex->SetType( BUMP );
					mat->AddTexture( tex );
					//AddTexture(m_name.c_str(), path.c_str(), BUMP, MODULATE, 1.0f, 1.0f, 1.0f, true, true);
				}
			}

			//bump texture
			if(m->GetTextureCount(aiTextureType_HEIGHT)>0)
			{
				unsigned int numTex = m->GetTextureCount(aiTextureType_HEIGHT);

				for(unsigned int j=0; j<numTex; ++j)
				{
					//Get ambient textures
					texFound = m->GetTexture(aiTextureType_HEIGHT, j, &pth);
					if(texFound==AI_FAILURE)
						break;

					path = "data/tex/normal/";
					path += pth.C_Str();

					TexturePtr tex = m_texture_cache->CreateFromImage( path.c_str() );
					tex->SetType( BUMP );
					mat->AddTexture( tex );
					//AddTexture(m_name.c_str(), path.c_str(), BUMP, MODULATE, 1.0f, 1.0f, 1.0f, true, true);
				}
			}
		}
	}//if(load_materials)

	//update load list
	UpdateLoadList(scene->mNumMeshes);

	cout<<"\nLoading scene ( "<<scene->mNumMeshes<<" objects ):"<<endl;
	cout<<"-------------------------------------------------------------------------------"<<endl;

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

		cout<<"Adding object "<<oname<< " ... ";

		m_obj_cache[oname] = m_objects[oname]->Create(mesh);

		//assign material
		if(load_materials)
		{
			o->SetMaterial( mats[mesh->mMaterialIndex] );
		}
		//set sceneID
		m_objects[oname]->SetSceneID(m_sceneID);

		LoadScreen();
	}

	cout<<"Scene loaded: " <<polygons<<" polygons.\n\n";
}
