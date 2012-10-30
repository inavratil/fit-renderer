/**
****************************************************************************************************
****************************************************************************************************
@file: draw.cpp
@brief drawing the scene
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"


/**
****************************************************************************************************
@brief Render scene - camera, lights and objects(run through object list and call render function
for every object)
@param delete_buffer should we delete color and depth buffer?
***************************************************************************************************/
void TScene::Redraw(bool delete_buffer)
{
    GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    ///draw all lights
    unsigned i;
    for(i=0, m_il = m_lights.begin(); m_il != m_lights.end(), i<m_lights.size(); ++m_il, i++)
    {
        ///if light has a shadow, render scene from light view to texture (TScene::RenderShadowMap())
        if((*m_il)->IsCastingShadow())
        {
            //render shadow map
            if((*m_il)->GetType() == OMNI)
            {
				RenderShadowMapOmni(*m_il);
            }
            else 
                RenderShadowMap(*m_il);
        }
    }

    //HDR/SSAO renderer - render to texture
    if(m_useHDR || m_useSSAO)
    {
        //render target viewport size
        glViewport(0,0,m_RT_resX,m_RT_resY);

        //attach framebuffer to render to
        glBindFramebuffer(GL_FRAMEBUFFER, m_f_buffer);
        //attach render texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["render_texture"], 0);

        ///use multisampled FBO if required
        if(m_msamples > 1)
            glBindFramebuffer(GL_FRAMEBUFFER, m_f_bufferMSAA);


        //multiple render targets - only when using SSAO and/or normal buffer
        if(m_useNormalBuffer)    
            glDrawBuffers(2, mrt);

        //clear screen (if desired)
        if(delete_buffer)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else        //else render to default framebuffer, clear it(if desired)
    {
        if(delete_buffer)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glViewport(0,0,m_RT_resX,m_RT_resY);

    //render all opaque objects
    DrawScene(DRAW_OPAQUE);

    //then transparent objects
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    DrawScene(DRAW_TRANSPARENT);
    glDisable(GL_BLEND);

    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    //HDR/SSAO renderer
    if(m_useHDR || m_useSSAO)
    {        
        //if MSAA enabled, copy from multisampled FBO to normal FBO
        if(m_msamples > 1)
        {
            //blit colors
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_f_bufferMSAA);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_f_buffer);
            glBlitFramebuffer(0, 0, m_resx, m_resy, 0, 0, m_resx, m_resy, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            //blit normals
            if(m_useNormalBuffer)
            {
                glReadBuffer(GL_COLOR_ATTACHMENT1);
                glDrawBuffer(GL_COLOR_ATTACHMENT1);
                glBlitFramebuffer(0, 0, m_resx, m_resy, 0, 0, m_resx, m_resy, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }
        }

        if(m_useNormalBuffer)
            glDrawBuffer(GL_COLOR_ATTACHMENT0);

        //attach bloom texture
        glBindFramebuffer(GL_FRAMEBUFFER, m_f_buffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["bloom_texture"], 0);  
 
        //downsample bloom texture by setting new viewport
        glViewport(0,0,m_RT_resX/2,m_RT_resY/2);
        //Bloom/SSAO pass
        RenderPass("mat_bloom_hdr_ssao");        
        //horizontal blur pass
        RenderPass("mat_blur_horiz");
        //vertical blur pass
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["blur_texture"], 0);
        RenderPass("mat_blur_vert");

        //go back to regular framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //final draw with bloom and tone mapping
        glViewport(0,0,m_resx,m_resy);  //restore original scene viewport
        RenderPass("mat_tonemap");
    }

    //show shadow maps
    if(m_draw_shadow_map)
    {
        for(int i=0; i<2; i++)
        {
            const float q_size = 0.5f;
            if(m_lights[0]->GetType() == OMNI)
            {
                SetUniform("show_depth_omni", "far_plane", SHADOW_FAR);
                SetUniform("show_depth_omni", "index", float(i));
                RenderSmallQuad("show_depth_omni", 0.0f, i*q_size, q_size);
            }
            else
            {
                SetUniform("show_depth", "far_plane", SHADOW_FAR);
                RenderSmallQuad("show_depth", 0.0f, 0.0f, q_size);
                break;
            }
        }
    }

    //finish drawing, restore buffers
    glBindVertexArray(0);
}

/**
****************************************************************************************************
@brief Draw all objects in scene. Drawing is done in material manner because shader switching is slow
***************************************************************************************************/
void TScene::DrawScene(int drawmode)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
    {
        if(!m_im->second->IsScreenSpace()) //don't render shaders working in screen space!
        {
            ///select drawing mode
            float transparent = m_im->second->GetTransparency();
            if(drawmode == DRAW_OPAQUE && transparent > 0.0)
                continue;
            else if(drawmode == DRAW_TRANSPARENT && transparent == 0.0)
                continue;
            else if(drawmode == DRAW_ALPHA && !m_im->second->IsAlpha())
                continue;

            ///attach material shader
            m_im->second->RenderMaterial();
            unsigned matID = m_im->second->GetID();

            ///render all objects attached to this material
            for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
            {
                if(m_io->second->GetSceneID() == m_sceneID && m_io->second->GetMatID() == matID)
                {
                    //update matrix
                    glm::mat4 m = m_viewMatrix * m_io->second->GetMatrix();

                    m_im->second->SetUniform("in_ModelViewMatrix", m);
                    m_io->second->Draw(m_im->second->IsTessellated()); //draw object
                }
            }
        }
    }
}

/**
*********************************************************************************************************
@brief Draw all objects in scene. Only depth values are outputted (drawing into shadow map for spot light)
********************************************************************************************************/
void TScene::DrawSceneDepth(const char* shadow_mat, glm::mat4& lightMatrix)
{
    //then other with depth-only shader
    m_materials[shadow_mat]->RenderMaterial();
    glActiveTexture(GL_TEXTURE0);
    m_materials[shadow_mat]->SetUniform("alpha_tex", 0);
    bool tess = m_materials[shadow_mat]->IsTessellated();

    //draw objects in mode according to their material
    for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
    {
        if(!m_im->second->IsScreenSpace() && m_im->second->GetTransparency() == 0.0)
        {
            //if there is alpha channel texture, attach it to depth shader
            if(m_im->second->IsAlpha())
            {
                m_materials[shadow_mat]->SetUniform("alpha_test", 1);                
                glBindTexture(GL_TEXTURE_2D, m_im->second->GetAlphaTexID());                
            }                

            ///get material ID and render all objects attached to this material
            unsigned matID = m_im->second->GetID();
            for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
            {
                if(m_io->second->IsShadow() && m_io->second->GetSceneID() == m_sceneID 
                && m_io->second->GetMatID() == matID)
                {
                    //update matrix
                    glm::mat4 m = lightMatrix * m_io->second->GetMatrix();
                    m_materials[shadow_mat]->SetUniform("in_ModelViewMatrix", m );
                    m_io->second->Draw(tess);
                }
            }
            //disable alpha test (if was enabled)
            if(m_im->second->IsAlpha())
                m_materials[shadow_mat]->SetUniform("alpha_test", 0);
        }
    }
}


/**
****************************************************************************************************
@brief Draw loading screen
***************************************************************************************************/
void TScene::LoadScreen(bool swap)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //TODO: RenderPass("mat_load_background");
    //DrawScreenText("Initializing, please wait... ",50.0f,45.0f,1.2f);

    //progress bar
    m_load_actual--;
    float loaded;
    if(m_load_actual > 0)
        loaded = -0.7f + (float)(m_load_list - m_load_actual)/m_load_list;
    else
        loaded = 0.7f;
    
    //draw progress bar
    if(m_materials["mat_progress_bar"]->IsShaderOK())
        m_materials["mat_progress_bar"]->RenderMaterial();
    
    GLfloat vertattribs[] = { -0.7f,-0.2f, loaded,-0.2f, -0.7f,-0.3f, loaded,-0.3f };

    glBindVertexArray(SceneManager::Instance()->getVBO(VBO_ARRAY, "progress_bar"));
    glBindBuffer(GL_ARRAY_BUFFER, SceneManager::Instance()->getVBO(VBO_BUFFER, "progress_bar"));
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_STREAM_DRAW); 
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    if(swap)
     SDL_GL_SwapBuffers();
}
