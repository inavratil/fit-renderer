/**
****************************************************************************************************
****************************************************************************************************
@file: render_target.cpp
@brief handles rendering to texture/cube maps for dynamic reflections/refractions
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"

#include "resources/SceneManager.h"
/**
****************************************************************************************************
@brief Create screen quad
***************************************************************************************************/
void TScene::AddScreenQuad()
{
	VBO tmp_vbo;

    glGenVertexArrays(1, &tmp_vbo.vao);
    glBindVertexArray(tmp_vbo.vao);

    //vertex attributes for screen quad
    GLfloat vertattribs[] = { -1.0,1.0, 1.0,1.0, -1.0,-1.0, 1.0,-1.0 };
    glGenBuffers(1, &tmp_vbo.buffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, tmp_vbo.buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_STATIC_DRAW); 
    //bind attributes to index
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

	SceneManager::Instance()->setVBO("screen_quad", tmp_vbo);

    //add also small quad
    GLfloat texattribs[] = { 0.0,1.0, 1.0,1.0, 0.0,0.0, 1.0,0.0 };

    glGenVertexArrays(1, &tmp_vbo.vao);
    glBindVertexArray(tmp_vbo.vao);

    //vertex attribs...
    glGenBuffers(1, &tmp_vbo.buffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, tmp_vbo.buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_DYNAMIC_DRAW); 
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //and also texcoords
    glGenBuffers(1, &tmp_vbo.buffer[1]);
    glBindBuffer(GL_ARRAY_BUFFER, tmp_vbo.buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &texattribs, GL_STATIC_DRAW); 
    glVertexAttribPointer(GLuint(1), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

	SceneManager::Instance()->setVBO("small_quad", tmp_vbo);
}

/**
****************************************************************************************************
@brief Render screen quad with attached material
@param material shader
***************************************************************************************************/
void TScene::RenderPass(const char* material)
{
    //render material and quad covering whole screen
	m_material_manager->GetMaterial(material)->RenderMaterial();
    glBindVertexArray( SceneManager::Instance()->getVBO(VBO_ARRAY, "screen_quad") );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	m_material_manager->GetMaterial(material)->DectivateTextures();
}


/**
****************************************************************************************************
@brief Render small quad over scene with attached material (to visualize buffers etc...)
@param material shader
@param offset_x X-location on the screen (0 - 1)
@param offset_y Y-location on the screen (0 - 1)
@param size quad size (0 - 1)
***************************************************************************************************/
void TScene::RenderSmallQuad(const char* material, float offset_x, float offset_y, float size)
{
    //update vertex attributes
    float left = -1.0f + offset_x; 
    float right = left + size;
    float bottom = -1.0f + offset_y; 
    float top = bottom + size; 
    GLfloat vertattribs[] = { left, top, right, top, left, bottom, right, bottom };

    //render material and quad covering whole screen
    m_material_manager->GetMaterial(material)->RenderMaterial();

    glBindVertexArray( SceneManager::Instance()->getVBO(VBO_ARRAY, "small_quad") );
    glBindBuffer(GL_ARRAY_BUFFER,SceneManager::Instance()->getVBO(VBO_BUFFER, "small_quad", 0 ) );
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_DYNAMIC_DRAW);   //update vertex data
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);  //bind attributes to index

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	m_material_manager->GetMaterial(material)->DectivateTextures();
}

/**
****************************************************************************************************
@brief Check framebuffer status
@return is FBO complete?
***************************************************************************************************/
bool TScene::CheckFBO()
{
    //check FBO creation
    GLenum FBOstat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstat != GL_FRAMEBUFFER_COMPLETE)
    {
        if(FBOstat ==  GL_FRAMEBUFFER_UNDEFINED) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_UNDEFINED");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        if(FBOstat ==  GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        if(FBOstat ==  GL_FRAMEBUFFER_UNSUPPORTED) 
            ShowMessage("FBO ERROR:GL_FRAMEBUFFER_UNSUPPORTED");
        return false;
    }
    return true;
}

/**
****************************************************************************************************
@brief Resize render target textures
@param resX new X-size of framebuffer textures
@param resY new Y-size of framebuffer texture
@param tex_format OpenGL texture format (can be GL_RGBA, GL_RGBA16F or GL_RGBA32F)
@param tex_type internal data format (can be GL_UNSIGNED_BYTE or GL_FLOAT)
***************************************************************************************************/
void TScene::ResizeHDRRenderTarget(int resX, int resY, GLint tex_format, GLenum tex_type )
{
    //FBO must be attached, otherwise return
    if(m_f_buffer == NULL) return;

    //resize texture - for original image
    glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get( "render_texture" ));
    glTexImage2D(GL_TEXTURE_2D, 0, tex_format, resX, resY, 0, GL_RGBA, tex_type, NULL);

    //resize texture - for store normal values
    if(m_useNormalBuffer)
    {
        glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get( "normal_texture" ));
        glTexImage2D(GL_TEXTURE_2D, 0, tex_format, resX, resY, 0, GL_RGBA, tex_type, NULL);
    }

    //resize texture - for bloom effect
    glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get( "bloom_texture" ));
    glTexImage2D(GL_TEXTURE_2D, 0, tex_format, resX, resY, 0, GL_RGBA, tex_type, NULL);

    //resize texture - for blur
    glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get( "blur_texture" ));
    glTexImage2D(GL_TEXTURE_2D, 0, tex_format, resX, resY, 0, GL_RGBA, tex_type, NULL);

    //resize renderbuffer storage 
    glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,resX, resY);

    glBindFramebuffer(GL_FRAMEBUFFER, m_f_buffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_r_buffer_depth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //resize renderbuffer storage (multisampled)
    if(m_msamples > 1)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_colorMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msamples, GL_DEPTH_COMPONENT,resX, resY);

        glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_colorMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msamples, tex_format, resX, resY);

        if(m_useNormalBuffer)
        {
            glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_normalMSAA);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msamples, tex_format, resX, resY);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_f_bufferMSAA);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_r_buffer_colorMSAA);
        if(m_useNormalBuffer)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, m_r_buffer_normalMSAA);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_r_buffer_depthMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
