/**
****************************************************************************************************
****************************************************************************************************
@file: scene.cpp
@brief initialization and drawing the scene - definitions
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"

#include "resources/SceneManager.h"
#include "sdk/ScreenSpaceMaterial.h"

/**
****************************************************************************************************
@brief Initialize scene by clearing lists
****************************************************************************************************/
TScene::TScene()
{
    //FBO's
    m_useHDR = m_useSSAO = m_useNormalBuffer = false;
    m_f_buffer = m_r_buffer_depth = m_f_bufferMSAA = m_r_buffer_colorMSAA = m_r_buffer_depthMSAA = 0;
    m_msamples = 0;

    //scene
    m_dp_FOV = 179.0;   
    m_load_list = m_load_actual = 0;
 	m_wireframe = false;
    m_font2D_tex = 0;
    m_font2D_bkg = 0;
    m_lights.clear();
    m_sceneID = 0;
	m_cam = NULL;
    m_msamples = 1;

    //shadows
    m_useShadows = false;
    m_shadow_textures = 0;
    m_dpshadow_tess = false;

    m_use_pcf = true;
    m_dpshadow_method = WARP_DPSM;
    m_parab_angle = glm::vec3(0.0, 0.0, 0.0);
    m_cut_angle = glm::vec2(0.0);
	
	m_texture_cache = new TextureCache();
	m_material_manager = new MaterialManager();
	m_mrt_pass = NULL;
	
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
    GLuint to_delete[] = { SceneManager::Instance()->getVBO(VBO_ARRAY, "screen_quad"), SceneManager::Instance()->getVBO(VBO_ARRAY, "progress_bar") };
    glDeleteVertexArrays(2, to_delete);

    delete m_font2D_tex;
    delete m_font2D_bkg;

    delete m_cam;

	//-- delete managers and caches
	delete m_texture_cache;
	delete m_material_manager;
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
bool TScene::PreInit(GLint resx, GLint resy, int msamples, bool load_font)
{
    m_resx = resx;
    m_resy = resy;
    m_msamples = msamples;
    if(msamples < 1) msamples = 1;              //don't accept 0 for multisample count

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    glClearColor(0.1, 0.1, 0.1, 0.5); //clear color and depth
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

	ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_progress_bar", "data/shaders/quad.vert", "data/shaders/progress_bar.frag" );
	mat->AddTexture( m_texture_cache->CreateFromImage( "data/load.png" ), "tex" );
	m_material_manager->AddMaterial( mat );
	
	TexturePtr render_texture = m_texture_cache->Create2DManual( 
		"render_texture",
		m_resx, m_resy,
		GL_RGBA16F,
		GL_FLOAT,
		GL_NEAREST,
		false
		);
	TexturePtr normal_texture = m_texture_cache->Create2DManual( 
		"normal_texture",
		m_resx, m_resy,
		GL_RGBA16F,
		GL_FLOAT,
		GL_NEAREST,
		false
		);

	//m_mrt_pass = new SimplePass( m_resx, m_resy );
	//m_mrt_pass->AttachOutputTexture( 0, render_texture );
	//m_mrt_pass->AttachOutputTexture( 1, normal_texture );
	//m_mrt_pass->Validate();

    //add screen quad for render targets
    AddScreenQuad();

	m_material_manager->AddMaterial( new ScreenSpaceMaterial( "mat_default_quad","data/shaders/quad.vert", "data/shaders/quad.frag" ) );

    return true;
}


/**
****************************************************************************************************
@brief Scene second initialization. The purpose of this initialization is to assign default material
to objects without material and create shadow maps(TScene::CreateShadowMap()) for lights with shadows
enabled, and to add this shadow map(GeometryMaterial::AddShadowMap()) to all materials
@return success/fail of initialization
****************************************************************************************************/
bool TScene::PostInit()
{    
	//-------------------------------------------------------------------------
    //-- Generate uniform buffers 
    
    //for projection and shadow matrices
    glGenBuffers(1, &m_uniform_matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
    glBufferData(GL_UNIFORM_BUFFER, (m_shadow_textures + 1) * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
	//update uniform buffer with projection matrix and camera position
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
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


	//-------------------------------------------------------------------------
    //do we have shadows from lights?

	for( m_it_render_listeners = m_render_listeners.begin();
		m_it_render_listeners != m_render_listeners.end();
		m_it_render_listeners++
		)
	{
		if(!(*m_it_render_listeners)->Initialize()) return false;
	}

    int i;
    for(i = 0, m_il = m_lights.begin(); m_il != m_lights.end(); ++m_il, i++)
    {
        if((*m_il)->IsCastingShadow()) //if yes, create shadow map for current light
        {
			if( m_dpshadow_method == CUT || m_dpshadow_method == DPSM )
				if(!CreateShadowMap(m_il)) return false;
        }

        //fill uniform block with light settings. Be careful to offsets!!!
		glBufferSubData( GL_UNIFORM_BUFFER, i*align, sizeof(glm::vec3), glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4((*m_il)->GetPos(), 1.0))) );
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
    

	//-------------------------------------------------------------------------

    cout<<"\nBaking materials ...\n";
	cout<<"-------------------------------------------------------------------------------"<<endl;

    //assign light count to all materials and then bake them
	for(m_material_manager->Begin(); 
		!m_material_manager->End();
		m_material_manager->Next())
	{
		Material* mat = m_material_manager->GetItem();
        if(mat->GetSceneID() == m_sceneID)
        {
            //set MRT if we use rendering to normal buffer
            //if(m_useNormalBuffer)
            //   mat->UseMRT(true);
            //bake material
            if( !mat->BakeMaterial(m_lights.size(), m_dpshadow_method, m_use_pcf) )
				throw ERR;

            LoadScreen();
        }
    }

	//-------------------------------------------------------------------------
	    
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
	m_projMatrix = glm::perspective(m_cam->GetFOVy(),(GLfloat)resx/resy,m_cam->GetNearPlane(),m_cam->GetFarPlane());
}



/**
****************************************************************************************************
@brief Destroys all dynamic scene data, but keeps scene settings (like perspective, clipping planes)
@param delete_cache should we delete also loaded textures from cache? (can be useful when next scene
uses the same textures)
***************************************************************************************************/
void TScene::Destroy(bool delete_cache)
{
    for(m_io = m_objects.begin(); m_io != m_objects.end(); m_io++)
        delete m_io->second;
	m_objects.clear();
    for(m_il = m_lights.begin(); m_il != m_lights.end(); m_il++)
        delete *m_il;
	m_lights.clear();
   
    
    if(delete_cache)
    {
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
    //m_cam->Reset();
    //delete buffers
    //GLuint to_delete[] = { m_screen_quad, m_progress_bar, m_uniform_matrices, m_uniform_lights };
    //glDeleteBuffers(4, to_delete);
}

/**
****************************************************************************************************
@brief Add 3DS object to scene. If objects has been loaded, a object pointer from cache is used
instead of reloading from file
@param name object name
@param file 3DS file with data
***************************************************************************************************/
TObject* TScene::AddObject(const char *name, const char* file)
{
    ///add new object
    TObject *o = new TObject();
    m_objects[name] = o;
	UpdateLoadList( 1 );

	cout<<"Adding object "<<name<< " ... ";

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

	return o;
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
TLight* TScene::AddLight(GLint _lights, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 lpos, GLfloat radius)
{ 
    //-- name
    string m_name = "default_light_" + num2str(m_lights.size());
	//-- material
	GeometryMaterial* mat = new GeometryMaterial( m_name.c_str() );
	mat->SetColor( 2.0f*diff, 2.0f*diff, 2.0f*diff );
	mat->SetShininess( 0.0 );
	mat->ReceiveShadow( false );
	m_material_manager->AddMaterial( mat );
	//-- object for light
    TObject* obj = AddObject(m_name.c_str(), "data/obj/light.3ds");
	obj->SetMaterial( mat->GetID() );
	obj->CastShadow( false );

    //create new and push into list
    TLight *l = new TLight(_lights, amb, diff, spec, lpos, radius);
    m_lights.push_back(l); 
    //update position
    MoveLight(_lights, lpos);

	return l;
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
        m_objects[l_name.c_str()]->MoveAbs(w.x, w.y, w.z);
		Material* mat = m_material_manager->GetMaterial( l_name.c_str() );
        if(mat->IsShaderOK())
        {
            mat->SetUniform( "in_ModelViewMatrix",m_viewMatrix * m_objects[l_name.c_str()]->GetMatrix());
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
