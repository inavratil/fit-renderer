/**
****************************************************************************************************
****************************************************************************************************
@file: scene.cpp
@brief initialization and drawing the scene - definitions
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"

/**
****************************************************************************************************
@brief Initialize scene by clearing lists
****************************************************************************************************/
TScene::TScene()
{
    //FBO's
    m_useHDR = m_useSSAO = m_useNormalBuffer = false;
    m_f_buffer = m_r_buffer_depth = m_f_bufferMSAA = m_r_buffer_colorMSAA = m_r_buffer_depthMSAA = 0;
    m_aerr_buffer = NULL;
    m_msamples = 0;

    //scene
    m_dp_FOV = 179.0;   
    m_load_list = m_load_actual = 0;
 	m_wireframe = false;
    m_font2D_tex = 0;
    m_font2D_bkg = 0;
    m_lights.clear();
    m_sceneID = 0;
    m_cam = new TCamera();
    m_custom_cam = false;
    m_msamples = 1;

    //shadows
    m_useShadows = false;
    m_shadow_textures = 0;
    m_dpshadow_tess = false;
    m_draw_shadow_map = false;
    m_min_depth = 0.0;
    m_avg_depth = 100.0;
    m_max_depth = 1000.0;
    m_select_buffer = 0;

    m_use_pcf = true;
    m_dpshadow_method = DPSM;
    m_draw_aliasError = false;
    m_parab_angle = glm::vec3(0.0, 0.0, 0.0);
    m_cut_angle = glm::vec2(0.0);
	m_num_lines = 0;
}

/**
****************************************************************************************************
@brief Destructor. Free dynamic data
****************************************************************************************************/
TScene::~TScene()
{
    Destroy(true);
    //delete font
    glDeleteLists(m_font2D, 256);
    //delete buffers
    GLuint to_delete[] = { m_screen_quad.vao, SceneManager::Instance()->getVBO(VBO_ARRAY, "progress_bar") };
    glDeleteVertexArrays(2, to_delete);

    delete [] m_select_buffer;
    delete [] m_aerr_buffer;

    /*
    materials.clear();
    m_objects.clear();
    m_lights.clear();
    */
//    delete [] select_buffer;

    //free all objects, materials, textures...
    for(m_im = m_materials.begin(); m_im != m_materials.end(); m_im++)
        delete m_im->second;
    for(m_io = m_objects.begin(); m_io != m_objects.end(); m_io++)
        delete m_io->second;
    for(m_il = m_lights.begin(); m_il != m_lights.end(); m_il++)
        delete *m_il;

    delete m_font2D_tex;
    delete m_font2D_bkg;

    delete m_cam;
}

/**
****************************************************************************************************
@brief Scene first initialization. In this function it's being checked if accelerator has all necessary
extensions to run program. Then are set basic scene variables like screen resolution, near and far
clipping planes and perspective correction.
@param resx screen width in pixels
@param resy screen height in pixels
@param _near near clipping plane (must be greater than 0.0)
@param _far far clipping plane (must be greater than near)
@param fovy viewing angle (in degrees)
@param msamples number of samples used in multisampling
@param cust_cam it's possible to use a custom camera instead of scene camera
@param load_font shall we load font?
@return success/fail of initialization
****************************************************************************************************/
bool TScene::PreInit(GLint resx, GLint resy, GLfloat _near, GLfloat _far, GLfloat fovy, 
                     int msamples, bool cust_cam, bool load_font)
{
    m_resx = resx;
    m_resy = resy;
    m_near_p = _near;
    m_far_p = _far;
    m_fovy = fovy;
    m_custom_cam = cust_cam;
    m_msamples = msamples;
    if(msamples < 1) msamples = 1;              //don't accept 0 for multisample count

    glClearColor(0.0, 0.0, 0.0, 0.5); //clear color and depth
    glEnable(GL_DEPTH_TEST);          //enable depth buffer
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    //create initial scene size
    Resize(resx,resy);

    //initialize font
    if(load_font)
        BuildFont();
    
    //progress bar and background
	VBO tmp_vbo;
  	
	glGenVertexArrays(1, &tmp_vbo.vao);	  	
	glBindVertexArray(tmp_vbo.vao);

	glGenBuffers(1, &tmp_vbo.buffer[0]);
  	glBindBuffer(GL_ARRAY_BUFFER, tmp_vbo.buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); 
    glBindVertexArray(0);

	SceneManager::Instance()->setVBO("progress_bar", tmp_vbo);
    
    AddMaterial("mat_progress_bar", white, white, white, 0.0, 0.0, 0.0, SCREEN_SPACE);
    AddTexture("mat_progress_bar","data/load.tga");
    CustomShader("mat_progress_bar","data/shaders/quad.vert", "data/shaders/progress_bar.frag");

    return true;
}


/**
****************************************************************************************************
@brief Scene second initialization. The purpose of this initialization is to assign default material
to objects without material and create shadow maps(TScene::CreateShadowMap()) for lights with shadows
enabled, and to add this shadow map(TMaterial::AddShadowMap()) to all materials
@return success/fail of initialization
****************************************************************************************************/
bool TScene::PostInit()
{    
    //Generate uniform buffers 
    
    //for projection and shadow matrices
    glGenBuffers(1, &m_uniform_matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
    glBufferData(GL_UNIFORM_BUFFER, (m_shadow_textures + 1) * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	//attach uniform buffer and associate uniform block to this name
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_MATRICES, m_uniform_matrices);

    //for lights
    unsigned light_count = m_lights.size();
    if(light_count == 0)
    {
        ShowMessage("There must be at least one light in the scene. Exiting.");
        return false;
    }
    glGenBuffers(1, &m_uniform_lights);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights);
    //buffer size: 4x vec3 values + 1x float value multiplied by lights count; 1 vec3 value
    glBufferData(GL_UNIFORM_BUFFER, light_count * 6 * align, NULL, GL_STREAM_DRAW);
	//attach uniform buffer and associate uniform block to this name
	glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_LIGHTS, m_uniform_lights);
    
    //do we have shadows from lights?
    int i;
    for(i = 0, m_il = m_lights.begin(); m_il != m_lights.end(); ++m_il, i++)
    {
        if((*m_il)->IsCastingShadow()) //if yes, create shadow map for current light
        {
			if( m_dpshadow_method == CUT || m_dpshadow_method == DPSM )
				if(!CreateShadowMap(m_il)) return false;
			if( m_dpshadow_method == WARP_DPSM )
				if(!CreateShadowMapWarped(m_il)) return false;

            //add shadow map to all materials (except those who don't receive shadows)
            for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
                if(m_im->second->GetSceneID() == m_sceneID && !m_im->second->IsScreenSpace())
                    m_im->second->AddShadowMap((*m_il)->GetType(), *(*m_il)->GetShadowTexID(), (*m_il)->ShadowIntensity() );

            CreateDataTexture("select_texture", Z_SELECT_SIZE, Z_SELECT_SIZE, GL_RGBA16F, GL_FLOAT, GL_TEXTURE_2D);
            //create render target for depth calculations
            glGenFramebuffers(1, &m_f_buffer_select);
            glBindFramebuffer(GL_FRAMEBUFFER, m_f_buffer_select);
            //attach texture to the frame buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["select_texture"], 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //add shader for selecting depth values (IPSM only)
            if((*m_il)->GetType() == OMNI)
            {
                if(!m_useNormalBuffer)  //normal buffer must be enabled before we can use IPSM
                    CreateHDRRenderTarget(-1, -1, GL_RGBA16F, GL_FLOAT, true);
                AddMaterial("mat_depth_select",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
                AddTexture("mat_depth_select", "normal_texture", RENDER_TEXTURE);
                CustomShader("mat_depth_select","data/shaders/quad.vert", "data/shaders/select_depth.frag");
                SetUniform("mat_depth_select","near_far",glm::vec2(m_near_p, m_far_p));

                //allocate z-selection buffer
                m_select_buffer = new float[Z_SELECT_SIZE*Z_SELECT_SIZE];
            }
        }

        //fill uniform block with light settings. Be careful to offsets!!!
        int offset = light_count * align;
        glBufferSubData(GL_UNIFORM_BUFFER, offset + i*align, sizeof(glm::vec3), glm::value_ptr((*m_il)->GetColor(AMBIENT)));   //ambient
        offset += light_count * align;
        glBufferSubData(GL_UNIFORM_BUFFER, offset + i*align, sizeof(glm::vec3), glm::value_ptr((*m_il)->GetColor(DIFFUSE)));   //diffuse
        offset += light_count * align;
        glBufferSubData(GL_UNIFORM_BUFFER, offset + i*align, sizeof(glm::vec3), glm::value_ptr((*m_il)->GetColor(SPECULAR)));   //specular
        offset += light_count * align;
        GLfloat radius = (*m_il)->GetRadius();
        glBufferSubData(GL_UNIFORM_BUFFER, offset + i*align, sizeof(float), &radius);   //radius
    }
    
    CreateAliasErrorTarget();

    cout<<"Baking materials...\n";
    //assign light count to all materials and then bake them
    for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
    {
        if(m_im->second->GetSceneID() == m_sceneID)
        {
            //set MRT if we use rendering to normal buffer
            if(m_useNormalBuffer)
                m_im->second->UseMRT(true);
            //bake material
            m_im->second->BakeMaterial(m_lights.size(), m_dpshadow_method, m_use_pcf);
            LoadScreen();
        }
    }

    //add screen quad for render targets
    AddScreenQuad();

    //update uniform buffer with projection matrix and camera position
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
    UpdateCameraUniform();

	
	//create renderbuffers
	GLuint fbo, tmp;

	CreateDataTexture("MTEX_debug_output", 1024, 1024, GL_RGBA16F, GL_FLOAT);

	glGenRenderbuffers(1, &tmp);
	glBindRenderbuffer(GL_RENDERBUFFER, tmp);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
	//attach texture to the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["MTEX_debug_output"], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, tmp);
	m_fbos["debug_fbo"] = fbo;

		
	//check FBO creation
    if(!CheckFBO())
    {
        ShowMessage("ERROR: FBO creation for shadow map failed!",false);
        return false;
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cout<<"Post Init OK\n";
    return true;
}


/**
****************************************************************************************************
@brief Resize screen to new dimensions, re-initialize viewport and projection matrices
@param _resx X-resolution in pixels
@param _resy Y-resolution in pixels
***************************************************************************************************/
void TScene::Resize(GLint resx, GLint resy)
{
    m_resx = resx;
    m_resy = resy;
    glViewport(0,0,resx,resy);        //new viewport settings
    m_projMatrix = glm::perspective(m_fovy,(GLfloat)resx/resy,m_near_p,m_far_p);
}



/**
****************************************************************************************************
@brief Destroys all dynamic scene data, but keeps scene settings (like perspective, clipping planes)
@param delete_cache should we delete also loaded textures from cache? (can be useful when next scene
uses the same textures)
***************************************************************************************************/
void TScene::Destroy(bool delete_cache)
{
    m_objects.clear();
    m_materials.clear();
    m_lights.clear();
    m_fbos.clear();

    if(delete_cache)
    {
        //delete textures from cache
        for(m_it = m_tex_cache.begin(); m_it != m_tex_cache.end(); ++m_it)
            glDeleteTextures(1, &m_it->second);
        
        m_tex_cache.clear();
        m_obj_cache.clear();
    }

    //delete framebuffers
    if(m_useHDR || m_useSSAO)
    {
        glDeleteFramebuffers(1, &m_f_buffer);
        glDeleteFramebuffers(1, &m_f_bufferMSAA);
        glDeleteFramebuffers(1, &m_f_buffer_select);
        glDeleteRenderbuffers(1, &m_r_buffer_depth);
        glDeleteRenderbuffers(1, &m_r_buffer_colorMSAA);
        glDeleteRenderbuffers(1, &m_r_buffer_depthMSAA);
    }

    //reset camera
    m_cam->Reset();
    //delete buffers
    //GLuint to_delete[] = { m_screen_quad, m_progress_bar, m_uniform_matrices, m_uniform_lights };
    //glDeleteBuffers(4, to_delete);
}


/**
****************************************************************************************************
//@brief Adds one texture to all materials
****************************************************************************************************/
void TScene::AddTextureToMaterials(const char *texname, const char *file, GLint textype, GLint texmode,
                                    GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso)
{
    for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
    {
        ///find out, if texture hasn't been loaded yet
        m_it = m_tex_cache.find(texname);
        GLint cache;
        ///if no match, load texture normally
        if(m_it == m_tex_cache.end() )
            cache = -1;
        ///else use existing texture
        else
            cache = m_it->second;
        ///then save texture ID to cache
        int texID;
        if( (texID = m_im->second->LoadTexture( texname, textype, file, texmode, intensity, tileX, tileY, mipmap, aniso, cache)) == ERR)
            throw ERR;
        m_tex_cache[texname] = texID;
    }
}

/**
****************************************************************************************************
@brief Add texture to scene. If texture has been loaded, a texture pointer from cache is used
instead of reloading from file
@param name texture name
@param file external texture file (.tga)
@param textype texture type (can be BASE,ENV,BUMP,PARALLAX,DISPLACE,CUBEMAP,CUBEMAP_ENV, ALPHA, SHADOW,
                                    SHADOW_OMNI, RENDER_TEXTURE, RENDER_TEXTURE_MULTISAMPLE)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param mipmap should we generate mipmaps?
@param aniso should we use anisotropic filtering?
***************************************************************************************************/
void TScene::AddTexture(const char *name, const char *file, GLint textype, GLint texmode,
                        GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso)
{
    //material existence control
    if(m_materials.find(name) == m_materials.end())
    {
        cerr<<"WARNING (AddTexture): no material with name "<<name<<"\n";
        return;
    }
    ///find out, if texture hasn't been loaded yet
    m_it = m_tex_cache.find(file);
    GLint cache;
    ///if no match, load texture normally
    if( m_it == m_tex_cache.end() )
        cache = -1;
    ///else use existing texture
    else
        cache = m_it->second;
    ///then save texture ID to cache
    int texID;
    if( (texID = m_materials[name]->AddTexture(file,textype,texmode,intensity,tileX,tileY,mipmap,aniso,cache)) == ERR)
        throw ERR;
    else
        m_tex_cache[file] = texID;
    //LoadScreen();	//update loading screen
}


/**
****************************************************************************************************
@brief Add cubemap texture to scene. If texture has been loaded, a texture pointer from cache is used
instead of reloading from file
@param name texture name
@param files array of 6 external texture files (.tga)
@param textype texture type (CUBEMAP)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param aniso should we use anisotropic filtering?
***************************************************************************************************/
void TScene::AddTexture(const char *name, const char **files, GLint textype, GLint texmode,
                        GLfloat intensity, GLfloat tileX, GLfloat tileY, bool aniso)
{
    //material existence control
    if(m_materials.find(name) == m_materials.end())
    {
        cerr<<"WARNING (AddTexture): no material with name "<<name<<"\n";
        return;
    }

    //find out, if texture hasn't been loaded yet
    m_it = m_tex_cache.find(files[0]);
    GLint cache;
    ///if no match, load texture normally
    if( m_it == m_tex_cache.end() )
        cache = -1;
    ///else use existing texture
    else
        cache = m_it->second;
    ///then save texture ID to cache
    int texID;
    if( (texID = m_materials[name]->AddTexture(files,textype,texmode,intensity,tileX,tileY,aniso,cache)) == ERR)
        throw ERR;
    m_tex_cache[files[0]] = texID;
    //LoadScreen();	//update loading screen
}

/**
****************************************************************************************************
@brief Add texture from given data
@param name material name
@param tex_name texture name
@param data texture data in any format (int, float...)
@param tex_size texture width and height
@param tex_format OpenGL texture format (can be GL_RGBA, GL_RGBA16F or GL_RGBA32F)
@param data_type internal data format (can be GL_UNSIGNED_BYTE or GL_FLOAT)
@param textype texture type (can be BASE,ENV,BUMP)
@param texmode texture addition mode (can be ADD,MODULATE,DECAL,BLEND)
@param intensity texture color intensity (or bump intensity)
@param tileX count of horizontal tiles
@param tileY count of vertical tiles
@param mipmap sholud we generate mipmaps for texture?
@param aniso should we use anisotropic filtering?
***************************************************************************************************/
void TScene::AddTextureData(const char *name, const char *tex_name, const void *data, glm::vec2 tex_size, GLenum tex_format, GLenum data_type, 
        GLint textype, GLint texmode, GLfloat intensity, GLfloat tileX, GLfloat tileY, bool mipmap, bool aniso)
{
    //material existence control
    if(m_materials.find(name) == m_materials.end())
    {
        cerr<<"WARNING (AddTextureData): no material with name "<<name<<"\n";
        return;
    }
    //save texture ID to cache
    int texID;
    if( (texID = m_materials[name]->AddTextureData(tex_name,textype,data,tex_size,tex_format,data_type,
                                                   texmode,intensity,tileX,tileY,mipmap,aniso)) == ERR)
        throw ERR;
    else
        m_tex_cache[tex_name] = texID;
    //LoadScreen();	//update loading screen
}

/**
****************************************************************************************************
@brief Bind material(identified by mat_name) to object(identified by obj_name)
@param obj_name object name
@param mat_name material name
***************************************************************************************************/
void TScene::SetMaterial(const char* obj_name, const char *mat_name)
{
    //object existence control
    if(m_objects.find(obj_name) == m_objects.end())
    {
        cerr<<"WARNING (SetMaterial): no object with name "<<obj_name<<"\n";
        return;
    }
    //material existence control
    if(m_materials.find(mat_name) == m_materials.end())
    {
        cerr<<"WARNING (SetMaterial): no object with name "<<mat_name<<"\n";
        return;
    }
    m_objects[obj_name]->SetMaterial(m_materials[mat_name]->GetID());
}

/**
****************************************************************************************************
@brief Add 3DS object to scene. If objects has been loaded, a object pointer from cache is used
instead of reloading from file
@param name object name
@param file 3DS file with data
***************************************************************************************************/
void TScene::AddObject(const char *name, const char* file)
{
    ///add new object
    TObject *o = new TObject();
    m_objects[name] = o;

    ///find out, if object hasn't been loaded yet
    m_iob = m_obj_cache.find(file);

    ///if no match, load object normally
    if( m_iob == m_obj_cache.end() )
    {
        VBO vbo_ret = m_objects[name]->Create(name,file,true);
        if(vbo_ret.vao == 0)
            throw ERR;
        m_obj_cache[file] = vbo_ret;
    }
    ///else use existing object data
    else
    {
        m_objects[name]->SetVBOdata(m_iob->second);   //must be called before create!
        m_objects[name]->Create(name,file,false);
    }
    LoadScreen();	//update loading screen
    //set sceneID
    m_objects[name]->SetSceneID(m_sceneID);
}


/**
****************************************************************************************************
@brief Add new light object. New light is pushed to the end of the list
@param _lights light index
@param amb ambient color
@param diff diffuse color
@param spec specular color
@param lpos light position
@param radius light faloff radius
***************************************************************************************************/
void TScene::AddLight(GLint _lights, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 lpos, GLfloat radius)
{ 
    //object for light
    string m_name = "default_light_" + num2str(m_lights.size());
    AddMaterial(m_name.c_str(), 2.0f*diff, 2.0f*diff, 2.0f*diff, 0.0, 0.0, 0.0);
    AddObject(m_name.c_str(), "data/obj/light.3ds");
    SetMaterial(m_name.c_str(), m_name.c_str());
    ObjCastShadow(m_name.c_str(), false);
    MatReceiveShadow(m_name.c_str(), false);

    //create new and push into list
    TLight *l = new TLight(_lights, amb, diff, spec, lpos, radius);
    m_lights.push_back(l); 
    //update position
    MoveLight(_lights, lpos);
}


/**
****************************************************************************************************
@brief Move light identified by number to new absolute position
@param light light index
@param w new light position
***************************************************************************************************/
void TScene::MoveLight(GLint light, glm::vec3 w)
{
    if(light < 0 || (unsigned)light > m_lights.size()) 
        cerr<<"WARNING: no light with index "<<light<<"\n";
    else 
    {
        m_lights[light]->Move(w);
        string l_name = "default_light_" + num2str(light);
        //update light position
        MoveObjAbs(l_name.c_str(), w.x, w.y, w.z);
        if(m_materials[l_name.c_str()]->IsShaderOK())
        {
            SetUniform(l_name.c_str(), "in_ModelViewMatrix",m_viewMatrix * m_objects[l_name.c_str()]->GetMatrix());
            //update uniform buffer
            glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights);
            glBufferSubData(GL_UNIFORM_BUFFER, light*align, sizeof(glm::vec3), glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4(w, 1.0)))); 
        }
    }
}

/**
****************************************************************************************************
@brief Change color of light(identified by index)
@param light light index
@param component light component(GL_AMBIENT,GL_DIFFUSE or GL_SPECULAR)  (see TLight::ChangeColor() )
@param color new RGB color
***************************************************************************************************/
void TScene::ChangeLightColor(GLint light, GLint component, glm::vec3 color)
{
    if(light < 0 || (unsigned)light > m_lights.size()) 
        cerr<<"WARNING: no light with index "<<light<<"\n";
    else 
    {
        m_lights[light]->ChangeColor(component,color);
        //update uniform buffer
        int offset1 = m_lights.size()*align;
        int offset2 = light*align;
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights);
        glBufferSubData(GL_UNIFORM_BUFFER, component*offset1 + offset2, sizeof(glm::vec3), glm::value_ptr(color)); 
    }
}
