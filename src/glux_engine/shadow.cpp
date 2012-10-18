/**
****************************************************************************************************
****************************************************************************************************
@file: shadow.cpp
@brief shadow mapping algoritmus
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"


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
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    
    glm::vec3 look_point = glm::vec3( glm::inverse(m_viewMatrix) * glm::vec4(0.0f, 0.0f, 1.0, 1.0));
    float zoom[MAX_CASCADES];
    bool in_frustum = true;

    //OPTIMAL VIEW FRUSTUM COVERAGE CALCULATION
    float FOV = 0.0;
    
    //TWO PASS - FRONT AND BACK
    for(int i=0; i<2; i++)
    {
        if(!in_frustum && i == 1)    //if light is outside view frustum, draw only front paraboloid
            break;

        zoom[i] = 1.0f;

        float z_direction = 1.0;
        if(i == 1)
            z_direction = -1.0;                  

        {
            //look point at original DPSM
            lightViewMatrix[i] = glm::lookAt(
                l->GetPos(), 
                l_pos + glm::vec3(glm::vec4(glm::vec3(m_far_p*z_direction, 0.0f, 0.0f), 1.0f)),                   
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

        ///All scene is drawn without materials and lighting (only depth values and alpha tests are needed)
        //set light position and zoom        
        if(m_dpshadow_tess)
        {
            m_materials["_mat_default_shadow_omni_tess"]->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
            m_materials["_mat_default_shadow_omni_tess"]->SetUniform("ZOOM", zoom[i]);

            DrawSceneDepth("_mat_default_shadow_omni_tess", lightViewMatrix[i]);
        }
        else
        {
            m_materials["_mat_default_shadow_omni"]->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
            m_materials["_mat_default_shadow_omni"]->SetUniform("ZOOM", zoom[i]);

            DrawSceneDepth("_mat_default_shadow_omni", lightViewMatrix[i]);
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


    }

}
