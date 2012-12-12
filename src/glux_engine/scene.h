/**
****************************************************************************************************
****************************************************************************************************
@file: scene.h
@brief initialization and drawing the scene - declarations
****************************************************************************************************
***************************************************************************************************/
#ifndef _SCENE_H_
#define _SCENE_H_

#include "globals.h"
#include "material.h"
#include "light.h"
#include "camera.h"
#include "shadow.h"

#include "SceneManager.h"

const int align = sizeof(glm::vec4);      //BUG: ATI Catalyst 10.12 drivers align uniform block values to vec4

/**
@class TScene
@brief containts list of objects, inits and draws 3D scene.
It has 3 main lists: objects, materials and lights. Materials are being applied to objects and
all this is lit by lights.
***************************************************************************************************/
class TScene
{
protected:
    ///associative array with all objects
    map<string,TObject*> m_objects;
    ///iterator for objects container
    map<string,TObject*>::iterator m_io;

    ///associative array with all materials
    map<string,TMaterial*> m_materials;
    ///iterator for materials container
    map<string,TMaterial*>::iterator m_im;
    
	///associative array with all lights
    vector<TLight*> m_lights;
    ///iterator for lights container
    vector<TLight*>::iterator m_il, m_il2;
    
	///associative array with all FBOs
    map<string,GLuint> m_fbos;
    ///iterator for fbos container
    map<string,GLuint>::iterator m_ifbo;

    int m_shadow_textures;    //count of shader textures

    ///texture cache: all loaded textures are stored here; if there's request for load of already
    ///loaded texture, cache texture will be used. Cache contains source file and pointer to
    ///already created texture
    map<string,GLuint> m_tex_cache;
    ///iterator for texture cache container
    map<string,GLuint>::iterator m_it;
    ///3DS objects cache - purpose is the same as texture cache
    map<string,VBO> m_obj_cache;
    ///iterator for object cache container
    map<string,VBO>::iterator m_iob;

    ///uniform buffers
    GLuint m_uniform_matrices, m_uniform_lights;

    ///2D font texture
    Texture *m_font2D_tex;
    ///Background texture
    Texture *m_font2D_bkg;
    ///Display list for font
    GLuint m_font2D;

    //scene parameters

    ///screen resolution
    GLint m_resx, m_resy;
    ///clipping planes
    GLfloat m_near_p, m_far_p;
    ///field of view
    GLfloat m_fovy;

    ///scene camera
    TCamera *m_cam;
    ///do we use custom camera?
    bool m_custom_cam;
    ///number of multisaples in antialiasing
    GLint m_msamples;

    ///camera matrices
    glm::mat4 m_viewMatrix, m_projMatrix;

    //scene render to texture target (FBO) and progress bar
    VBO m_screen_quad, m_small_quad;

    ///shall we use HDR, SSAO or shadows?
    bool m_useHDR, m_useSSAO, m_useShadows, m_useNormalBuffer;
    ///wireframe
    bool m_wireframe;

    ///framebuffer/renderbuffer objects for color/depth
    GLuint m_f_buffer, m_r_buffer_depth, m_f_buffer_select,
           m_f_bufferMSAA, m_r_buffer_colorMSAA, m_r_buffer_normalMSAA, m_r_buffer_depthMSAA;


    ///render target sizes
    GLint m_RT_resX, m_RT_resY;				

    ///number of items to be loaded
    int m_load_list, m_load_actual;

    ///subscene ID - used when we need to hide some parts of the scene
    int m_sceneID;

    ///settings for DPSM
    float m_dp_FOV;
    glm::vec3 m_parab_angle;
    bool m_dpshadow_tess, m_draw_shadow_map;
    bool m_use_pcf, m_draw_aliasError;
    int m_dpshadow_method;
    float m_min_depth, m_avg_depth, m_max_depth;
    glm::vec3 m_avg_normal;
    float *m_select_buffer, *m_aerr_buffer;
    glm::vec2 m_cut_angle;
    GLuint m_aerr_f_buffer, m_aerr_f_buffer_color, m_aerr_r_buffer_depth;
	//FIXME
	int m_num_lines;
	int m_texPreview_id;
	bool m_warping_enabled;

public:
    //basic constructor
    TScene();
    //free dynamic data
    ~TScene();

    //scene initialization
    bool PreInit(GLint resx, GLint resy, GLfloat near, GLfloat far, GLfloat fovy, 
                 GLint msamples, bool cust_cam = false, bool load_font = true);
    bool PostInit();
    //resize window
    void Resize(GLint resx, GLint resy);
    //draw scene - setup, render targets and objects
    void Redraw(bool delete_buffer = true);
    //draw all objects in scene
    void DrawScene(int drawmode);
    void DrawSceneDepth(const char* shadow_mat, glm::mat4& lightMatrix);
    void DrawAliasError(const char* alias_mat, glm::mat4& lightMatrix);

    //draw load screen
    void LoadScreen(bool swap = true);
    //destroy scene, free dynamic data
    void Destroy(bool delete_cache = true);
    ///@brief set load items count (extern objects, textures and scenes)
    void SetLoadList(int count){ 
        m_load_list = count; m_load_actual = count; 
    }
    ///@brief update load list (when loading scene)
    void UpdateLoadList(int count){  
        m_load_list += count; m_load_actual += count; 
    }

    //load whole scene from 3DS file
    void LoadScene(const char* file, bool load_materials = true, bool load_lights = true, string name_space = "");

    ///Change scene ID. All objects will from now belong to this ID. Only one sceneID section can be active at time
    void ChangeSceneID(int id){  
        m_sceneID = id; 
    }

    ///Toggle wireframe rendering
    void Wireframe(bool flag){  
        m_wireframe = flag; 
    }

    /////////////////////////////////////////// CAMERA ////////////////////////////////////////

    ///@brief Update new camera position and modelview matrix(for lights) in uniform buffer
    void UpdateCameraUniform(){
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights);
        //glBufferSubData(GL_UNIFORM_BUFFER, lights.size()*5*align, sizeof(glm::vec3), glm::value_ptr(cam.GetPos())); 
        int i; //update light positions with new modelview matrix
        for(i = 0, m_il = m_lights.begin(); m_il != m_lights.end(); ++m_il, ++i)
            glBufferSubData( GL_UNIFORM_BUFFER, i*align, sizeof(glm::vec3), 
                             glm::value_ptr(glm::vec3(m_viewMatrix * glm::vec4((*m_il)->GetPos(), 1.0))) );
    }

    ///@brief Move camera to new position(relative)
    void MoveCamera(GLfloat wx, GLfloat wy, GLfloat wz){ 
        m_viewMatrix = m_cam->Move(wx,wy,wz);
        UpdateCameraUniform();
    }
    ///@brief Move camera to new position(absolute)
    void MoveCameraAbs(GLfloat wx, GLfloat wy, GLfloat wz){ 
        m_viewMatrix = m_cam->MoveAbs(wx,wy,wz); 
        UpdateCameraUniform();
    }

    ///@brief Camera look
    void LookCameraAt(GLfloat wx, GLfloat wy, GLfloat wz){
        m_viewMatrix = m_cam->LookAt(wx,wy,wz); 
        UpdateCameraUniform();
    }

    ///@brief Rotate camera around axis(A_X, A_Y or A_Z) by angle(relative)
    void RotateCamera(GLfloat angle, GLint axis){ 
        m_viewMatrix =  m_cam->Rotate(angle,axis);
        UpdateCameraUniform();
    }
    ///@brief Rotate camera around axis(A_X, A_Y or A_Z) by angle(absolute)
    void RotateCameraAbs(GLfloat angle, GLint axis){ 
        m_viewMatrix =  m_cam->RotateAbs(angle,axis); 
        UpdateCameraUniform();
    }

    ///@brief Switch between custom and scene camera
    void SwitchCamera(){ 
        m_custom_cam = !m_custom_cam; 
    }

    ///@brief Print out camera position
    void PrintCamera(){ 
        cout<<"POS: "<<m_cam->GetPos().x<<","<<m_cam->GetPos().y<<","<<m_cam->GetPos().z<<"\n"
            <<"ROT: "<<m_cam->GetRot().x<<","<<m_cam->GetRot().y<<","<<m_cam->GetRot().z<<"\n"; 
    }

    ///@brief Get screen-space camera position
    glm::vec3 GetCameraPos(){ 
        return m_cam->GetPos(); 
    }

    ///@brief Get camera rotation
    glm::vec3 GetCameraRot(){ 
        return m_cam->GetRot();
    }

    ///@brief Change camera type
    void SetCamType(int type){
        m_cam->SetType(type);
    }

    //Save camera into file
    void SaveCamera(){
        m_cam->Save();
    }
    //Load camera position
    void LoadCamera(){
        m_viewMatrix = m_cam->Load();
        UpdateCameraUniform();
    }


    /////////////////////////////////////////// LIGHTS ////////////////////////////////////////

    void AddLight(GLint _lights, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 lpos, GLfloat radius = 1000.0);

    ///@brief Remove light identified by number
    ///@todo remove selected light only, not whole list
    void RemoveLight(/*int lightnum*/){ 
        m_lights.clear(); 
    }

    void MoveLight(GLint light, glm::vec3 w);
    void ChangeLightColor(GLint light, GLint component, glm::vec3 color);

    ///@brief Return number of lights in scene
    int GetLightCount(){ 
        return m_lights.size(); 
    }

    ///@brief Return light position
    glm::vec3 GetLightPos(int light){
        if(light < 0 || (unsigned)light >= m_lights.size()) { cerr<<"WARNING: no light with index "<<light<<"\n"; return glm::vec3(0.0); }
        else return m_lights[light]->GetPos();
    }

    ///@brief set light radius
    void SetLightRadius(int light, GLfloat radius){
        if(light < 0 || (unsigned)light >= m_lights.size()) 
            cerr<<"WARNING: no light with index "<<light<<"\n";
        else {
            m_lights[light]->SetRadius(radius);
            //update uniform buffer
            glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_lights);
            glBufferSubData(GL_UNIFORM_BUFFER, m_lights.size()*4*align + light*align, sizeof(float), &radius); 
        }
    }

    /////////////////////////////////////////// OBJECTS ////////////////////////////////////////

    ///@brief Add new object (see TObject() into list. Index is object name
    void AddObject(const char *name, int primitive, GLfloat size = 0.0, GLfloat height = 0.0, GLint sliceX = 1, GLint sliceY = 1){
        TObject *o = new TObject(name, primitive, size, height, sliceX, sliceY);
        m_objects[name] = o;
        LoadScreen(); //update loading screen
        m_objects[name]->SetSceneID(m_sceneID);
    }
    ///@brief add new object as instance from existing object
    void AddObjectInstance(const char *ref_name, const char *inst_name){
        if(m_objects.find(ref_name) == m_objects.end()) 
            cerr<<"WARNING (AddObjectInstance): no reference object with name "<<ref_name<<"\n";
        else{
            m_objects[inst_name]->CreateInstance(*m_objects[ref_name]);
            m_objects[inst_name]->SetSceneID(m_sceneID);
        }
    }
    //add new object from external file
    void AddObject(const char *name, const char* file);

    ///@brief Move object identified by name to new position(relative) (see TObject::Move() )
    void MoveObj(const char* name, GLfloat wx, GLfloat wy, GLfloat wz){ 
        m_objects[name]->Move(wx,wy,wz); 
    }
    ///@brief Move object identified by name to new position(absolute) (see TObject::MoveAbs() )
    void MoveObjAbs(const char* name, GLfloat wx, GLfloat wy, GLfloat wz){ 
        m_objects[name]->MoveAbs(wx,wy,wz); 
    }

    ///@brief Rotate object identified by name around axis(can be A_X, A_Y, A_Z) by angle(relative)
    ///(see TObject::Rotate() )
    void RotateObj(const char* name, GLfloat angle, GLint axis){ 
        m_objects[name]->Rotate(angle,axis); 
    }
    ///@brief Rotate object identified by name around axis(can be A_X, A_Y, A_Z) by angle(absolute)
    ///(see TObject::RotateAbs() )
    void RotateObjAbs(const char* name, GLfloat angle, GLint axis){ 
        m_objects[name]->RotateAbs(angle,axis); 
    }

    ///@brief Resize object identified by name according to resize factor
    void ResizeObj(const char* name, GLfloat sx, GLfloat sy, GLfloat sz){ 
        m_objects[name]->Resize(sx,sy,sz); 
    }
    ///@brief Return object's vertex buffer ID
    GLint GetVertexBuffer(const char* name){
        return m_objects[name]->GetVertexBuffer(); 
    }


    ///@brief Get object position
    glm::vec3 GetObjPosition(const char *name){
        return m_objects[name]->GetPosition(); 
    }

    ///@brief toggle object drawing
    void DrawObject(const char* obj_name, bool flag){
        m_objects[obj_name]->DrawObject(flag); 
    }

    ///@brief Set count of object instances
    void SetGInstances(const char* obj_name, int count){ 
        m_objects[obj_name]->SetGInstances(count); 
    }



    /////////////////////////////////////// MATERIALS&TEXTURES /////////////////////////////////

    ///@brief Add new material(see TMaterial()) to list
    void AddMaterial(const char* name, glm::vec3 amb = black, glm::vec3 diff = silver, glm::vec3 spec = white,
        GLfloat shin = 64.0, GLfloat reflect = 0.0, GLfloat transp = 0.0, GLint lm = PHONG){
            TMaterial *m = new TMaterial(name, m_materials.size(), amb, diff, spec, shin, reflect, transp, lm);
            m_materials[name] = m;
            m_materials[name]->SetSceneID(m_sceneID);
            LoadScreen(); //update loading screen
    }
    //add texture to materials
    void AddTextureToMaterials(const char *texname, const char *file, GLint textype = BASE, GLint texmode = MODULATE,
        GLfloat intensity = 1.0, GLfloat tileX = 1.0, GLfloat tileY = 1.0, bool mipmap = true, bool aniso = false);

    //add texture
    void AddTexture(const char *name, const char *file, GLint textype = BASE, GLint texmode = MODULATE,
        GLfloat intensity = 1.0, GLfloat tileX = 1.0, GLfloat tileY = 1.0, bool mipmap = true, bool aniso = false);

    //add cubemap
    void AddTexture(const char *name, const char **files, GLint textype = CUBEMAP, GLint texmode = MODULATE,
        GLfloat intensity = 1.0, GLfloat tileX = 1.0, GLfloat tileY = 1.0, bool aniso = false);

    //add texture from data
    void AddTextureData(const char *mat_name, const char *tex_name, const void *data, glm::vec2 tex_size, GLenum tex_format = GL_RGB, GLenum data_type = GL_UNSIGNED_BYTE, 
        GLint textype = BASE, GLint texmode = MODULATE, GLfloat intensity = 1.0, GLfloat tileX = 1.0, GLfloat tileY = 1.0, bool mipmap = true, bool aniso = false);


    //bind material to object
    void SetMaterial(const char* obj_name, const char *mat_name);
    ///@brief Set material transparency
    void SetTransparency(const char* mat_name, GLfloat value){
        if(m_materials.find(mat_name) == m_materials.end())
            cerr<<"WARNING (SetTransparency): no material with name "<<mat_name<<"\n";
        else
            m_materials[mat_name]->SetTransparency(value);
    }

    ///@brief Load custom shader from file. Shader can be then applied to any material
    ///(see TMaterial::CustomShader() ). Vertex and fragment shader only
    void CustomShader(const char *name, const char* vert_source, const char* frag_source, 
                      const char *vert_defines = " ", const char *frag_defines = " "){
        if(m_materials.find(name) == m_materials.end())
            cerr<<"WARNING (CustomShader): no material with name "<<name<<"\n";
        else
            if(!m_materials[name]->CustomShader(vert_source, frag_source, vert_defines, frag_defines))
                throw ERR;
    }
    ///@brief Load custom shader from file. Shader can be then applied to any material
    ///(see TMaterial::CustomShader() ). Geometry and tesselaltion shaders are optional
    void CustomShader(const char *name, TShader *vertex, TShader *tess_control, TShader *tess_eval, TShader *geometry, TShader *fragment){
        if(m_materials.find(name) == m_materials.end())
            cerr<<"WARNING (CustomShader): no material with name "<<name<<"\n";
        else
            if(!m_materials[name]->CustomShader(vertex, tess_control, tess_eval,geometry, fragment))
                throw ERR;
    }

    ///@brief Set uniform variable in material shader (see TMaterial::SetUniform() )
    template<class UNIFORM>
    void SetUniform(const char* m_name, const char* v_name, UNIFORM value){ 
        if(m_materials.find(m_name) == m_materials.end())
            cerr<<"WARNING (SetUniform): no material with name "<<m_name<<"\n";
        else
            m_materials[m_name]->SetUniform(v_name, value);
    }

    ///@brief Force regeneration of all materials
    void BakeAllMaterials(){ 
        for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
            m_im->second->BakeMaterial(m_lights.size()); 
    }

    ///@brief Set material color
    void SetMaterialColor(const char *name, int component, glm::vec3 color){
        if(m_materials.find(name) == m_materials.end())
            cerr<<"WARNING (SetMaterialColor): no material with name "<<name<<"\n";
        else
            m_materials[name]->SetColor(component, color);
    }



    ////////////////////////////////////////////SHADOWS ///////////////////////////////////////

    //create render target for alias error 
    bool CreateAliasErrorTarget();
    //render alias error
    void RenderAliasError();
    //create shadow map for selected light
    bool CreateShadowMap(vector<TLight*>::iterator ii);
    //render shadow map (spot)
    void RenderShadowMap(TLight *l);
    //render shadow map (omnidirectional, dual-paraboloid)
    void RenderShadowMapOmni(TLight *l);

    //prepare textures and FBOs for multiresolution rendering
    bool CreateShadowMapWarped(vector<TLight*>::iterator ii);
    //render omnidirectional shadow map using multiresolution techniques
    void RenderShadowMapOmniWarped(TLight *l);
	//add additional vertex data
	void AddVertexDataWarped();
	void GeneratePolynomialGrid( glm::mat4 _coeffsX, glm::mat4 _coefssY, glm::vec4 _range );

    ///@brief Set shadow parameters(shadow size and intensity) for selected light (by index)
    ///(see TLight::SetShadow()
    void SetShadow(GLint lightNum, GLint shadow_size = 2048, int type = SPOT, 
                   GLfloat _shadow_intensity = 0.5, bool shadow = true){
        m_useShadows = true;
        if(lightNum < 0 || (unsigned)lightNum > m_lights.size()) 
            cerr<<"WARNING: no light with index "<<lightNum<<"\n";
        else 
        {
            m_lights[lightNum]->SetShadow(shadow_size,_shadow_intensity, type, shadow);
            m_shadow_textures++;
        }
    }

	//FIXME: NAVRH: presunout asi jinam, napr. do SceneManagera. Lepsi by byl zapis obj->DoCastShadow( flag )
    ///@brief Enable/disable shadow casting by selected object (by name) (see TObject::CastShadow() )
    void ObjCastShadow(const char *obj_name, bool flag){ 
        if(m_objects.find(obj_name) == m_objects.end()) 
            cerr<<"WARNING (cast shadow): no object with name"<<obj_name<<"!\n"; 
        else 
            m_objects[obj_name]->CastShadow(flag); 
    } 

	//FIXME: NAVRH:  presunout asi jinam, napr. do SceneManagera. Lepsi by byl zapis mat->DoReceiveShadow( flag )
    ///@brief Enable/disable shadow receiving for selected material (by name) (see TMaterial::ReceiveShadow() )
    void MatReceiveShadow(const char *mat_name, bool flag){ 
        if(m_materials.find(mat_name) == m_materials.end()) 
            cerr<<"WARNING (receive shadow): no material with name"<<mat_name<<"!\n"; 
        else 
            m_materials[mat_name]->ReceiveShadow(flag); 
    }

    ///@brief Change shadow paraboloid rotation
    void RotateParaboloid(glm::vec3 angle){
        m_parab_angle = angle;
    }


    ///@brief GLobally enable/disable shadows (for enable, shadows must be created for light that casts them
    void UseShadows(bool flag = true){ 
        m_useShadows = flag; 
    }

    //IMPROVED DUAL-PARABOLOID SHADOWS SETTINGS

    void DPSetCutAngle( glm::vec2 _angle ){
		m_cut_angle = _angle;
	}

    ///@brief set usage of PCF
    void DPSetPCF(bool flag){
        m_use_pcf = flag;
    }

    ///@brief Toggle dual-paraboloid  shadow enhancements
    void DPShadowMethod(int type){
        m_dpshadow_method = type;
        if(type == DPSM)
        {
            m_dp_FOV = 179.0f;
            m_avg_depth = 1000.0f;
        }
    }


    ///@brief Get far point value
    float DPGetFarPoint(){
        return m_avg_depth;
    }
    ///@brief Get average normal
    glm::vec3 DPGetAvgNormal(){
        return m_avg_normal;
    }

    ///@brief Get actual shadow FOV (front paraboloid)
    float DPGetFOV(){
        return m_dp_FOV;
    }

	bool DPGetTessellation()	  	
	{	  	
		return m_dpshadow_tess; 	  	
	}

    ///@brief toggle tessellation in paraboloid projection
    void DPSetTessellation(bool flag){
        if(flag && !GLEW_ARB_gpu_shader5)
        {
            cout<<"OpenGL 4 not supported. Cannot enable tessellation :(\n";
            flag = false;
        }
        m_dpshadow_tess = flag;
    }

    ///@brief Toggle shadow map drawing
    void DPDrawSM(bool flag){
        m_draw_shadow_map = flag;
    }

    void DPDrawAliasError(bool flag){
        m_draw_aliasError = flag;
    }

	void SetTexturePreviewId( int _id )
	{
		m_texPreview_id = _id;
	}

	void SetWarping( bool _flag )
	{
		m_warping_enabled = _flag;
	}

    ////////////////////////// RENDER TARGETS, HDR AND SSAO ////////////////////////

    //prepare screen quad
    void AddScreenQuad();
    //create single data texture
    void CreateDataTexture(const char* name, int resX, int resY, GLint tex_format, GLenum tex_type, GLenum tex_target = GL_TEXTURE_2D, bool mipmaps = false);
    //prepare texture to render into for HDR rendering
    void CreateHDRRenderTarget(int resX = -1, int resY = -1, GLint tex_format = GL_RGBA16F, GLenum tex_type = GL_FLOAT, bool normal_buffer = false);
    //resize render target textures
    void ResizeHDRRenderTarget(int resX, int resY, GLint tex_format = GL_RGBA16F, GLenum tex_type = GL_FLOAT);
    //create fbo
    bool CreateFBO(const char* name, int resX, int resY, GLuint tex, int fbo_mode = DEPTH_ONLY );
    //check framebuffer status
    bool CheckFBO();
    //render screen quad with attached shader
    void RenderPass(const char* material);
    //render small quad over scene (to visualize some buffers etc)
    void RenderSmallQuad(const char* material, float offset_x, float offset_y, float size);
    ///@brief toggle use of HDR
    void UseHDR(bool flag = true){ 
        m_useHDR = flag; 
    }
    ///@brief toggle use of SSAO
    void UseSSAO(bool flag = true){ 
        m_useSSAO = flag; 
        m_useNormalBuffer = flag;        
    }


    ///////////////////////////////////// TEXT ////////////////////////////////////////////////

    //Draw text on screen
    void DrawScreenText(const char *s, float x, float y, float size = 1.0, bool center = true);
    void BuildFont();

    ////////////////////////////////////// MISCELLANEOUS //////////////////////////////////////

    ///@brief Get screen width
    int GetResX(){
        return m_resx; 
    }
    ///@brief Get screen height
    int GetResY(){ 
        return m_resy; 
    }
};


#endif
