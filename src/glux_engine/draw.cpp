/**
****************************************************************************************************
****************************************************************************************************
@file: draw.cpp
@brief drawing the scene
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"

#include "resources/SceneManager.h"
/**
****************************************************************************************************
@brief Render scene - camera, lights and objects(run through object list and call render function
for every object)
@param delete_buffer should we delete color and depth buffer?
***************************************************************************************************/
void TScene::Redraw(bool delete_buffer)
{
    GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	for( m_it_render_listeners = m_render_listeners.begin();
		m_it_render_listeners != m_render_listeners.end();
		m_it_render_listeners++
		)
	{
		(*m_it_render_listeners)->PreRender();
	}

	//-------------------------------------------------------------------------
    //-- draw all lights
	/*
    unsigned i;
    for(i=0, m_il = m_lights.begin(); m_il != m_lights.end(), i<m_lights.size(); ++m_il, i++)
    {
        ///if light has a shadow, render scene from light view to texture (TScene::RenderShadowMap())
        if((*m_il)->IsCastingShadow())
        {
            //render shadow map
            if((*m_il)->GetType() == OMNI)
            {

				if( m_dpshadow_method == CUT || m_dpshadow_method == DPSM )
					RenderShadowMapOmni(*m_il);
				if( m_dpshadow_method == WARP_DPSM )
					WarpedShadows_RenderShadowMap(*m_il); 
				
            }
            else 
                RenderShadowMap(*m_il);
        }
    }
	*/

	//-------------------------------------------------------------------------
    //-- HDR/SSAO renderer - render to texture

    if(m_useHDR || m_useSSAO)
    {
        //render target viewport size
        glViewport(0,0,m_RT_resX,m_RT_resY);

        //attach framebuffer to render to
        glBindFramebuffer(GL_FRAMEBUFFER, m_f_buffer);
        //attach render texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_cache->Get( "render_texture" ), 0);

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

	//-------------------------------------------------------------------------

	if( m_mrt_pass )
		m_mrt_pass->Activate();

	if(delete_buffer)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
    //render all opaque objects
    DrawScene(DRAW_OPAQUE);

    //then transparent objects
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    DrawScene(DRAW_TRANSPARENT);
    glDisable(GL_BLEND);

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
	if( m_mrt_pass )
		m_mrt_pass->Deactivate();

	//-------------------------------------------------------------------------
    //-- HDR/SSAO renderer

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
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_cache->Get( "bloom_texture" ), 0);  
 
        //downsample bloom texture by setting new viewport
        glViewport(0,0,m_RT_resX/2,m_RT_resY/2);
        //Bloom/SSAO pass
        RenderPass("mat_bloom_hdr_ssao");        
        //horizontal blur pass
        RenderPass("mat_blur_horiz");
        //vertical blur pass
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_cache->Get( "blur_texture" ), 0);
        RenderPass("mat_blur_vert");

        //go back to regular framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //final draw with bloom and tone mapping
        glViewport(0,0,m_resx,m_resy);  //restore original scene viewport
        RenderPass("mat_tonemap");
    }

	//-------------------------------------------------------------------------
	if( m_mrt_pass )
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get("render_texture") );                
		//glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get("MTEX_warped_depth_color"));
		//SetUniform("mat_quad_array", "index", 1.0);
		RenderPass("mat_default_quad");
		glBindTexture(GL_TEXTURE_2D, 0);
	}
    
	//-------------------------------------------------------------------------
	//-- show shadow maps 

	for( m_it_render_listeners = m_render_listeners.begin();
		m_it_render_listeners != m_render_listeners.end();
		m_it_render_listeners++
		)
	{
		(*m_it_render_listeners)->PostRender();
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
	for(m_material_manager->Begin(); 
		!m_material_manager->End();
		m_material_manager->Next())
	{
		Material* mat = m_material_manager->GetItem();
        if(!mat->IsScreenSpace()) //don't render shaders working in screen space!
        {
            ///select drawing mode
            float transparent = static_cast< GeometryMaterial*>(mat)->GetTransparency();
            if(drawmode == DRAW_OPAQUE && transparent > 0.0)
                continue;
            else if(drawmode == DRAW_TRANSPARENT && transparent == 0.0)
                continue;
            else if(drawmode == DRAW_ALPHA && !mat->GetAlphaTexture())
                continue;

            ///attach material shader
            mat->RenderMaterial();
            int matID = mat->GetID();

            ///render all objects attached to this material
            for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
            {
				if( m_io->second->GetName() == "default_light_0" )
					int QQ = 1;
                if(m_io->second->GetSceneID() == m_sceneID && m_io->second->GetMatID() == matID)
                {
                    //update matrix
                    glm::mat4 m = m_viewMatrix * m_io->second->GetMatrix();

                    mat->SetUniform("in_ModelViewMatrix", m);
					m_io->second->Draw(mat->HasTessellationShader()); //draw object
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
	Material* mat_shadow_mat = m_material_manager->GetMaterial( shadow_mat );
    mat_shadow_mat->RenderMaterial();
    //glActiveTexture(GL_TEXTURE0);
    //m_materials[shadow_mat]->SetUniform("alpha_tex", 0);
	bool tess = mat_shadow_mat->HasTessellationShader();

    //draw objects in mode according to their material
	for(m_material_manager->Begin(); 
		!m_material_manager->End();
		m_material_manager->Next())
	{
		Material* mat = m_material_manager->GetItem();

		if( mat->IsScreenSpace() ) continue;

        if(static_cast< GeometryMaterial*>(mat)->GetTransparency() == 0.0)
        {
            //if there is alpha channel texture, attach it to depth shader
            //if(m_im->second->IsAlpha())
            //{
            //    m_materials[shadow_mat]->SetUniform("alpha_test", 1);                
            //    glBindTexture(GL_TEXTURE_2D, m_im->second->GetAlphaTexID());                
            //}                

            ///get material ID and render all objects attached to this material
            int matID = mat->GetID();
            for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
            {
                if(m_io->second->IsShadowCaster() && m_io->second->GetSceneID() == m_sceneID 
                && m_io->second->GetMatID() == matID)
                {
                    //update matrix
                    glm::mat4 m = lightMatrix * m_io->second->GetMatrix();
                    mat_shadow_mat->SetUniform("in_ModelViewMatrix", m );
                    m_io->second->Draw(tess);
                }
            }
            //disable alpha test (if was enabled)
            if(mat->GetAlphaTexture())
                mat_shadow_mat->SetUniform("alpha_test", 0);
        }
    }
}


void TScene::DrawGeometry(const char* _shader, glm::mat4& _mvMatrix )
{
    //then other with depth-only shader
	Material* mat_shader = m_material_manager->GetMaterial( _shader );
	int tex_unit = mat_shader->RenderMaterial();

	for(m_material_manager->Begin(); 
		!m_material_manager->End();
		m_material_manager->Next())
	{
		Material* mat = m_material_manager->GetItem();
		if(!mat->IsScreenSpace()) //don't render shaders working in screen space!
		{
			TexturePtr tex_alpha = mat->GetAlphaTexture();
			//if there is alpha channel texture, attach it to depth shader
			if( tex_alpha )
			{
				mat_shader->SetUniform("alpha_test", 1);   
				
				glActiveTexture(GL_TEXTURE0 + tex_unit);
				//texture location must be updated regularly
				mat_shader->SetUniform( "alpha_tex", tex_unit++ );

				tex_alpha->Bind(); 
			}

			for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
			{
				if(m_io->second->IsShadowCaster() && m_io->second->GetSceneID() == m_sceneID && mat->GetID() == m_io->second->GetMatID())
				{
					//update matrix
					glm::mat4 m = _mvMatrix * m_io->second->GetMatrix();
					mat_shader->SetUniform("in_ModelViewMatrix", m);

					m_io->second->Draw(mat_shader->HasTessellationShader());
				}
			}

			//disable alpha test (if was enabled)
			if( tex_alpha )
			{
				mat_shader->SetUniform("alpha_test", 0);   
				tex_alpha->Unbind(); 
			}
		}
	}

	mat_shader->DectivateTextures();

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
	Material* mat_progress_bar = m_material_manager->GetMaterial( "mat_progress_bar" );
    mat_progress_bar->RenderMaterial();
    
    GLfloat vertattribs[] = { 
		-0.7f,	-0.2f, 
		loaded,	-0.2f, 
		-0.7f,	-0.3f, 
		loaded,	-0.3f };



    glBindVertexArray(SceneManager::Instance()->getVBO(VBO_ARRAY, "progress_bar"));
    glBindBuffer(GL_ARRAY_BUFFER, SceneManager::Instance()->getVBO(VBO_BUFFER, "progress_bar"));
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_STREAM_DRAW); 
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	mat_progress_bar->DectivateTextures();
    
    if(swap)
     SDL_GL_SwapBuffers();
}
