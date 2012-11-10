/**
****************************************************************************************************
****************************************************************************************************
@file: shadow.cpp
@brief shadow mapping algoritmus
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"
#include "../ellipse_params.h"

bool TScene::CreateAliasErrorTarget()
{
    CreateDataTexture("aliaserr_texture", m_resx, m_resy, GL_RGBA16F, GL_FLOAT);
    //create renderbuffers
    glGenRenderbuffers(1, &m_aerr_r_buffer_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_aerr_r_buffer_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,m_resx, m_resy);

    glGenFramebuffers(1, &m_aerr_f_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_aerr_f_buffer);
    
    //attach texture to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["aliaserr_texture"], 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_aerr_r_buffer_depth);

    //check FBO creation
    if(!CheckFBO())
    {
        ShowMessage("ERROR: FBO creation for shadow map failed!",false);
        return false;
    }

    // Go back to regular frame buffer rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_aerr_buffer = new float[m_resx*m_resy];

	return true;
}

void TScene::RenderAliasError()
{
    glEnable(GL_DEPTH_TEST);

    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_aerr_f_buffer);
    glViewport( 0, 0, m_resx, m_resy );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 m = m_viewMatrix;
    DrawAliasError("mat_aliasError", m);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}


/**
****************************************************************************************************
@brief Creates shadow map for selected light. Shadow map parameters are stored in TLight object
@param ii iterator to lights list
@return success/fail of shadow creation
****************************************************************************************************/
bool TScene::CreateShadowMap(vector<TLight*>::iterator ii)
{
    cout<<"Shadow Map for light "<<(*ii)->GetOrd()<<": "<<(*ii)->ShadowSize()<<"x"<<(*ii)->ShadowSize()<<endl;

    ///1. generate depth texture for shadow map with desired dimensions for current light. Use cubemap for omni light
    ///2. set depth texture parameters
    if((*ii)->GetType() == SPOT)
    {
        glGenTextures(1, (*ii)->GetShadowTexID());
        glBindTexture(GL_TEXTURE_2D, *(*ii)->GetShadowTexID());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (*ii)->ShadowSize(), (*ii)->ShadowSize(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
    else if((*ii)->GetType() == OMNI)
    {
        //Array of shadow textures (front and back for each cascade)
        glGenTextures(1, (*ii)->GetShadowTexID());
        glBindTexture(GL_TEXTURE_2D_ARRAY, *(*ii)->GetShadowTexID());
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, (*ii)->ShadowSize(), (*ii)->ShadowSize(), 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }

    ///4. create framebuffer object and bind shadow texture into it
    glGenFramebuffers(1, &(*ii)->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, (*ii)->GetFBO());
    //add shadow texture(s)
    if((*ii)->GetType() == OMNI)
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *(*ii)->GetShadowTexID(), 0, 0);    //attach first texture layer
    else
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *(*ii)->GetShadowTexID(), 0);
    glDrawBuffer(GL_NONE); 
    glReadBuffer(GL_NONE);
    //check FBO creation
    if(!CheckFBO())
    {
        ShowMessage("ERROR: FBO creation for shadow map failed!",false);
        return false;
    }

    return true;
}


/**
****************************************************************************************************
@brief Draws scene to shadow map (for spotlight)
@param l current light
****************************************************************************************************/
void TScene::RenderShadowMap(TLight *l)
{
    ///1. set light projection and light view matrix
    glm::mat4 lightProjMatrix = glm::perspective(90.0f, 1.0f, 1.0f, 1000.0f);
    glm::mat4 lightViewMatrix = glm::lookAt(l->GetPos(), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f) );


    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ///2.bind framebuffer to draw into and draw scene from light point of view
    glBindFramebuffer(GL_FRAMEBUFFER, l->GetFBO());

    ///draw only back faces of polygons
    glCullFace(GL_FRONT);
    glColorMask(0, 0, 0, 0);      //disable colorbuffer write
    glClear(GL_DEPTH_BUFFER_BIT);

    //set viewport, draw scene
    glViewport(0, 0, l->ShadowSize(), l->ShadowSize());

    ///All scene is drawn without materials and lighting (only depth values and alpha tests are needed)
    DrawSceneDepth("_mat_default_shadow", lightViewMatrix);

    //Finish, restore values
    glCullFace(GL_BACK);
    glColorMask(1, 1, 1, 1);
    glViewport(0, 0, m_resx, m_resy);         //reset viewport

    ///3. Calculate shadow texture matrix
    glm::mat4 shadowMatrix = biasMatrix * lightProjMatrix * lightViewMatrix * glm::inverse(m_viewMatrix);

    //restore projection matrix and set shadow matrices
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, (l->GetOrd() + 1) * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shadowMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/**
****************************************************************************************************
@brief Draws scene to shadow map with dual-paraboloid mapping (for omnidirectional light)
@param l current light
****************************************************************************************************/
void TScene::RenderShadowMapOmni(TLight *l)
{
    glm::vec3 l_pos = l->GetPos();
    glm::mat4 lightViewMatrix[2];

    //position of camera in eye space = 0.0
    glm::vec3 cam_pos = glm::vec3(0.0);
    //position of light in eye space
    glm::vec3 l_pos_eye =  glm::vec3(m_viewMatrix * glm::vec4(l_pos, 1.0));
    //size of near/far plane of view frustum
    //m_max_depth = 2*m_avg_depth;
    float far_hsize = m_max_depth * glm::tan(glm::radians(m_fovy/2.0f));
    float near_hsize = m_min_depth * glm::tan(glm::radians(m_fovy/2.0f));
    
    glm::vec3 cam_nearfar[2*F_POINTS];          //near/far points
    glm::vec3 nearfar_light[2*F_POINTS];        //vectors from near/far into light position in eye space
    glm::vec3 look_point = glm::vec3( glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 1.0, 1.0));
    float zoom[MAX_CASCADES];
    bool in_frustum = true;

    ////////////////////////////////////////////////////////////////////////////////
    //-- parameters for skewed cut

    glm::mat4 cut_matrix_X = glm::mat4( 1.0 );
    glm::mat4 cut_matrix_Y = glm::mat4( 1.0 );
    glm::vec4 cut_params = glm::vec4( -1, 1, -1, 1 );

	if( m_dpshadow_method == CUT )
    {
        cut_matrix_Y = glm::rotate( cut_matrix_Y, -m_cut_angle.x, glm::vec3(0, 1, 0));
        cut_matrix_X = glm::rotate( cut_matrix_X, -m_cut_angle.y, glm::vec3(1, 0, 0));

        cut_params.x = ellipse_params[(int)m_cut_angle.y + 64][(int)m_cut_angle.x + 64][0];
        cut_params.y = ellipse_params[(int)m_cut_angle.y + 64][(int)m_cut_angle.x + 64][1];
        cut_params.z = ellipse_params[(int)m_cut_angle.y + 64][(int)m_cut_angle.x + 64][2]; 
        cut_params.w = ellipse_params[(int)m_cut_angle.y + 64][(int)m_cut_angle.x + 64][3]; 
    }

    //cout << "Cut params: " << cut_params.x << ", " << cut_params.y << " | " << cut_params.z << ", " << cut_params.w << endl;

    ////////////////////////////////////////////////////////////////////////////////


    //OPTIMAL VIEW FRUSTUM COVERAGE CALCULATION
    float FOV = 0.0;
    glm::vec3 avg_direction = glm::vec3( 0.0 );
    if(m_dpshadow_method == IPSM)
    {
        //Test, if light is inside frustum                
        glm::vec4 l_pos_proj =  glm::vec4(m_projMatrix * glm::vec4(l_pos_eye, 1.0));
        l_pos_proj /= l_pos_proj.w;                
        if( l_pos_proj.x > -1.0 && l_pos_proj.x < 1.0 &&
            l_pos_proj.y > -1.0 && l_pos_proj.y < 1.0 &&
            l_pos_proj.z > -1.0 && l_pos_proj.z < 1.0 )
        {
            in_frustum = true;
            FOV = 179.0;        //we need to cover whole environment
        }
        else
        {
            in_frustum = false;
        }

        //position of far plane points in eye space                    
        cam_nearfar[0] = glm::vec3( far_hsize, -far_hsize, -m_max_depth);
        cam_nearfar[1] = glm::vec3( far_hsize,  far_hsize, -m_max_depth);
        cam_nearfar[2] = glm::vec3(-far_hsize,  far_hsize, -m_max_depth);
        cam_nearfar[3] = glm::vec3(-far_hsize, -far_hsize, -m_max_depth);
        //position of near plane points in eye space
        cam_nearfar[4] = glm::vec3( near_hsize, -near_hsize, -m_min_depth);
        cam_nearfar[5] = glm::vec3( near_hsize,  near_hsize, -m_min_depth);
        cam_nearfar[6] = glm::vec3(-near_hsize,  near_hsize, -m_min_depth);
        cam_nearfar[7] = glm::vec3(-near_hsize, -near_hsize, -m_min_depth);

        //vectors from near/far view frustum plane to light
        for(int j=0; j<2*F_POINTS; j++)
            nearfar_light[j] = glm::normalize(cam_nearfar[j] - l_pos_eye);

        //Calculate avereage direction of vectors from light to frustum points
        for(int j=0; j<2*F_POINTS; j++)
            avg_direction = avg_direction + nearfar_light[j];
        avg_direction = glm::normalize(avg_direction);
        
        //Find highest angle between average direction and light-to-frustum vector
        for(int j=0; j<2*F_POINTS; ++j)
        {
            float deg = 2.0f * glm::degrees( glm::acos( glm::dot( avg_direction, nearfar_light[j] )));
            if( deg < 180.0f && deg > FOV )
                FOV = deg;
        }        
    }


    //TWO PASS - FRONT AND BACK
    for(int i=0; i<2; i++)
    {
        if(!in_frustum && i == 1)    //if light is outside view frustum, draw only front paraboloid
            break;

        zoom[i] = 1.0f;

        float z_direction = 1.0;
        if(i == 1)
            z_direction = -1.0;                  

        if(m_dpshadow_method == IPSM)
        { 
            //looking point for light (for front paraboloid)
            look_point = glm::vec3( glm::inverse(m_viewMatrix) * glm::vec4(z_direction*(avg_direction) 
                                    + l_pos_eye, 1.0f) ); 

            //TODO: when light is inside frustum it is better if paraboloid is aimed directly to camera position
            if(in_frustum)
                look_point = glm::vec3( z_direction * glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, -1.0, 1.0f) ); 

            //save FOV
            m_dp_FOV = FOV;      //front

            //set light view matrix (camera look transformed into world space)
            lightViewMatrix[i] = glm::lookAt(
                l->GetPos(), 
                look_point, 
                glm::vec3(0.0f, 1.0f, 0.0f) );

            //map 0-180 range of FOV into paraboloid zoom
            glm::vec2 dp_zoomcut = glm::vec2( glm::tan( glm::radians(FOV/2.0f) ), 1.0f );
            dp_zoomcut = glm::normalize( dp_zoomcut );
            zoom[i] = dp_zoomcut.x/(dp_zoomcut.y + 1.0f);

            //modify paraboloid zoom with cascade
            //if(cascades > 1 && i != cascades)
            //    zoom[i] /= (1<<i);
        }
        else    //manual FOV
        {
            //look point at original DPSM
            lightViewMatrix[i] = glm::lookAt(
                l->GetPos(), 
                l_pos + glm::vec3(m_far_p*z_direction, 0.0f, 0.0f),                   
                glm::vec3(0.0f, 1.0f, 0.0f) );
            FOV = m_dp_FOV;

            glm::mat4 Mp = glm::mat4( 1.0 );
            Mp = glm::rotate( Mp, z_direction*m_parab_angle.x, glm::vec3(1, 0, 0));
            Mp = glm::rotate( Mp, m_parab_angle.y, glm::vec3(0, 1, 0));

            lightViewMatrix[i] = Mp * lightViewMatrix[i];

        }

        ///2.bind framebuffer to draw into and draw scene from light point of view
        glBindFramebuffer(GL_FRAMEBUFFER, l->GetFBO());
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *l->GetShadowTexID(), 0, i);

        ///draw only back faces of polygons
        glCullFace(GL_FRONT);
        glColorMask(0, 0, 0, 0);      //disable colorbuffer write
        glClear(GL_DEPTH_BUFFER_BIT);

        //set viewport, draw scene
        glViewport(0, 0, l->ShadowSize(), l->ShadowSize());
        glEnable(GL_CLIP_PLANE0);

		glm::vec4 tmp_params = cut_params;
		glm::mat4 tmp_mat = cut_matrix_Y;
		if( m_dpshadow_method==CUT )
		{
			if(i == 1)
			{
				tmp_params.x = -1.0f*cut_params.y;
				tmp_params.y = -1.0f*cut_params.x;
				tmp_mat = glm::inverse( tmp_mat );
			}
		}

		tmp_mat = tmp_mat * cut_matrix_X;

        ///All scene is drawn without materials and lighting (only depth values and alpha tests are needed)
        //set light position and zoom        
        if(m_dpshadow_tess)
        {
            m_materials["_mat_default_shadow_omni_tess"]->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
            m_materials["_mat_default_shadow_omni_tess"]->SetUniform("ZOOM", zoom[i]);
            m_materials["_mat_default_shadow_omni_tess"]->SetUniform("cut_params", tmp_params);
            m_materials["_mat_default_shadow_omni_tess"]->SetUniform("in_CutMatrix", tmp_mat );

			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            DrawSceneDepth("_mat_default_shadow_omni_tess", lightViewMatrix[i]);
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        else
        {
            m_materials["_mat_default_shadow_omni"]->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
            m_materials["_mat_default_shadow_omni"]->SetUniform("ZOOM", zoom[i]);
            m_materials["_mat_default_shadow_omni"]->SetUniform("cut_params", tmp_params);
            m_materials["_mat_default_shadow_omni"]->SetUniform("in_CutMatrix", tmp_mat);

			
			if(m_wireframe)
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            DrawSceneDepth("_mat_default_shadow_omni", lightViewMatrix[i]);
			if(!m_wireframe)
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }
    }

    //Finish, restore values
    glDisable( GL_CLIP_PLANE0 );
    glCullFace(GL_BACK);
    glColorMask(1, 1, 1, 1);
    glViewport(0, 0, m_resx, m_resy);         //reset viewport

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //set light matrices and near/far planes to all materials
    for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
    {
        m_im->second->SetUniform("lightModelView[0]", lightViewMatrix[0]);
        m_im->second->SetUniform("lightModelView[1]", lightViewMatrix[1]);
        m_im->second->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
        m_im->second->SetUniform("ZOOM[0]", zoom[0]);
        m_im->second->SetUniform("ZOOM[1]", zoom[1]);
        //im->second.SetUniform("ZOOM[2]", zoom[2]);

			m_im->second->SetUniform("in_CutMatrix[0]", cut_matrix_X);
			m_im->second->SetUniform("in_CutMatrix[1]", cut_matrix_Y);
			m_im->second->SetUniform("cut_params", cut_params);
    }

		m_materials["mat_aliasError"]->SetUniform("in_CutMatrix[0]", cut_matrix_X);
		m_materials["mat_aliasError"]->SetUniform("in_CutMatrix[1]", cut_matrix_Y);
		m_materials["mat_aliasError"]->SetUniform("cut_params", cut_params);
}
