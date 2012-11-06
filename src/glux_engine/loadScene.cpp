/**
****************************************************************************************************
****************************************************************************************************
@file: loadScene.cpp
@brief loads entire scene from 3DS file using lib3ds: http://code.google.com/p/lib3ds/
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"
#include <lib3ds/lib3ds.h>


/**
****************************************************************************************************
@brief Loads entire scene from 3DS file
@param file 3DS file with scene
@param load_materials shall we load materials?
@param load_lights shall we load lights?
@param name_space prefix in object name (when using same model scene in different scenes)
****************************************************************************************************/
void TScene::LoadScene(const char* file, bool load_materials, bool load_lights, string name_space)
{
    int i;
    //Load 3DS scene using lib3ds
    cout<<"\nLoading scene "<<file<<":";
    Lib3dsFile *scene;
    scene = lib3ds_file_open(file);
    if(!scene)
    {
        ShowMessage("Cannot open 3DS file with scene!\n",false);
        throw ERR;
    }

    //update load list
    UpdateLoadList(scene->nmeshes + 2*scene->nmaterials);

    //Setup camera(if any)
    if(scene->cameras != NULL)
    {
		ShowMessage("Loading camera\n", false);
		throw ERR;

        Lib3dsCamera *cam = scene->cameras[0];
        MoveCamera(-cam->position[0], -cam->position[2], cam->position[1]);
        LookCameraAt(cam->target[0], cam->target[1], cam->target[2]);
    }

    //Setup lights
    if(load_lights)
    {
        cout<<scene->nlights<<" lights, ";
        for(i = 0; i < scene->nlights; i++)
        {
            Lib3dsLight *l = scene->lights[i];
            glm::vec3 l_color(l->color[0], l->color[1], l->color[2]);
            glm::vec3 l_position(l->position[0], l->position[2], l->position[1]);
            AddLight(i, black, l_color, l_color, l_position);
            MoveLight(i, l_position);
        }
    }

    //Load materials&textures
    vector<string> mats;						//array with materials (to index them)
    if(load_materials)
    {
        cout<<scene->nmaterials<<" materials. Loading textures:\n";
        for(i = 0; i < scene->nmaterials; i++)
        {
            //materials
            Lib3dsMaterial *m = scene->materials[i];

            //remove not-allowed chars from name
            string m_name = name_space + m->name;
            for(unsigned i=0; i<m_name.length(); i++)
            {
                if(m_name[i] < 0 || (!isalpha(m_name[i]) && !isdigit(m_name[i])) || m_name[i] > 128)
                    m_name.replace(i,1,"");
            }
            cout<<"Adding material "<<m_name<<endl;

            AddMaterial(
                m_name.c_str(),												//material name
                glm::vec3(m->ambient[0], m->ambient[1], m->ambient[2]),		//ambient color
                glm::vec3(m->diffuse[0], m->diffuse[1], m->diffuse[2]),		//diffuse color
                glm::vec3(m->specular[0], m->specular[1], m->specular[2]),	//specular color
                256.0f - 256.0f*m->shininess								//shininess
                );
            mats.push_back(m_name);

            //textures
            string path;
            //1. BASE texture 
            if(strlen(m->texture1_map.name) > 0)
            {
                path = "data/tex/";
                path += m->texture1_map.name;
                path.replace(path.find_last_of('.') + 1,3,"tga");
                AddTexture(m_name.c_str(), path.c_str(), BASE, MODULATE, m->texture1_map.percent, m->texture1_map.scale[0], m->texture1_map.scale[1], true, true);
            }
            //2. BASE texture 
            if(strlen(m->texture2_map.name) > 0)
            {
                path = "data/tex/";
                path += m->texture2_map.name;
                path.replace(path.find_last_of('.') + 1,3,"tga");
                AddTexture(m_name.c_str(), path.c_str(), BASE, MODULATE, m->texture2_map.percent, m->texture2_map.scale[0], m->texture2_map.scale[1]);
            }
            //BUMP texture 
            if(strlen(m->bump_map.name) > 0)
            {
                path = "data/tex/normal/";
                path += m->bump_map.name;
                path.replace(path.find_last_of('.') + 1,3,"tga");
                AddTexture(m_name.c_str(), path.c_str(), BUMP, MODULATE, m->bump_map.percent, m->bump_map.scale[0], m->bump_map.scale[1]);
            }
            //ENV texture 
            if(strlen(m->reflection_map.name) > 0)
            {
                path = "data/tex/";
                path += m->reflection_map.name;
                path.replace(path.find_last_of('.') + 1,3,"tga");
                AddTexture(m_name.c_str(), path.c_str(), ENV, ADD, m->reflection_map.percent, m->reflection_map.scale[0], m->reflection_map.scale[1]);
            }
        }
    }

    cout<<"Adding "<<scene->nmeshes<<" objects, ";
    //Load objects
    int polygons = 0;
    for(i = 0; i < scene->nmeshes; i++)
    {
        //skip empty objects
        if(scene->meshes[i]->nfaces > 0)
        {
            polygons += scene->meshes[i]->nfaces;
            Lib3dsMesh *m = scene->meshes[i];
            //create object from mesh
            string oname = name_space + m->name;
            TObject *o = new TObject();
            m_objects[oname] = o;
            m_obj_cache[oname] = m_objects[oname]->Create(scene->meshes[i]);
            //assign material (by first face index)
            if(load_materials)
            {
                if(m->faces[0].material >= 0)
                    SetMaterial(oname.c_str(), mats[m->faces[0].material].c_str());
            }
            //set sceneID
            m_objects[oname]->SetSceneID(m_sceneID);
        }
        LoadScreen();	//update loading screen
    }
    cout<<polygons<<" polygons.\nScene loaded.\n\n";
}
