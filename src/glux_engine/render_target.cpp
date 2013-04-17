/**
****************************************************************************************************
****************************************************************************************************
@file: render_target.cpp
@brief handles rendering to texture/cube maps for dynamic reflections/refractions
****************************************************************************************************
***************************************************************************************************/
#include "scene.h"

/**
****************************************************************************************************
@brief Create screen quad
***************************************************************************************************/
void TScene::AddScreenQuad()
{
    glGenVertexArrays(1, &m_screen_quad.vao);
    glBindVertexArray(m_screen_quad.vao);

    //vertex attributes for screen quad
    GLfloat vertattribs[] = { -1.0,1.0, 1.0,1.0, -1.0,-1.0, 1.0,-1.0 };
    glGenBuffers(1, &m_screen_quad.buffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_screen_quad.buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_STATIC_DRAW); 
    //bind attributes to index
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //add also small quad
    GLfloat texattribs[] = { 0.0,1.0, 1.0,1.0, 0.0,0.0, 1.0,0.0 };

    glGenVertexArrays(1, &m_small_quad.vao);
    glBindVertexArray(m_small_quad.vao);

    //vertex attribs...
    glGenBuffers(1, &m_small_quad.buffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_small_quad.buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_DYNAMIC_DRAW); 
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //and also texcoords
    glGenBuffers(1, &m_small_quad.buffer[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_small_quad.buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &texattribs, GL_STATIC_DRAW); 
    glVertexAttribPointer(GLuint(1), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

/**
****************************************************************************************************
@brief Render screen quad with attached material
@param material shader
***************************************************************************************************/
void TScene::RenderPass(const char* material)
{
    //render material and quad covering whole screen
    m_materials[material]->RenderMaterial();
    glBindVertexArray(m_screen_quad.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	m_materials[material]->DectivateTextures();
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
    m_materials[material]->RenderMaterial();

    glBindVertexArray(m_small_quad.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_small_quad.buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &vertattribs, GL_DYNAMIC_DRAW);   //update vertex data
    glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);  //bind attributes to index

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
@brief Create single render texture
@param name texture name (as index in texture cache)
@param resX X-size of texture
@param resY Y-size of texture
@param tex_format OpenGL texture format (can be GL_RGBA, GL_RGBA16F or GL_RGBA32F)
@param tex_target Texture target. Can be GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE
@param tex_type internal data format (can be GL_UNSIGNED_BYTE or GL_FLOAT)
***************************************************************************************************/
/*
void TScene::CreateDataTexture(const char* name, int resX, int resY, GLint tex_format, GLenum tex_type, GLenum tex_target, bool mipmaps)
{
    //create texture
    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(tex_target, texid);

    GLenum min_filter = GL_NEAREST;
    if(mipmaps)
        min_filter = GL_NEAREST_MIPMAP_NEAREST;

    if(tex_target == GL_TEXTURE_2D_MULTISAMPLE)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_msamples, tex_format, resX, resY, GL_FALSE);
    else
    {
        //following properties can be set only in normal 2D textures
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameterf(tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, tex_format, resX, resY, 0, GL_RGBA, tex_type, NULL);
        if(mipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);
    }

    //add created textures into cache
    m_tex_cache[name] = texid;
}
*/

/**
****************************************************************************************************
@brief Create render target using framebuffer and attached texture
@param resX X-size of framebuffer texture
@param resY Y-size of framebuffer texture
@param tex_format OpenGL texture format (can be GL_RGBA, GL_RGBA16F or GL_RGBA32F)
@param tex_type internal data format (can be GL_UNSIGNED_BYTE or GL_FLOAT)
@param normal_buffer should we create another render target for normal and depth buffer?
***************************************************************************************************/
/*
void TScene::CreateHDRRenderTarget(int resX, int resY, GLint tex_format, GLenum tex_type, bool normal_buffer)
{
    //if no resolution given, use current scene resolution
    if(resX == -1 || resY == -1)
    {
        resX = m_resx;
        resY = m_resy;
    }
    //force use of normal buffer if SSAO or IPSM is enabled
    if(m_useSSAO || m_dpshadow_method >= IPSM) 
        normal_buffer = true;
    m_useNormalBuffer = normal_buffer;

    //save render target size
    m_RT_resX = resX;
    m_RT_resY = resY;

    //create texture - for original image
	m_texture_cache->Create2DManual( "render_texture", resX, resY, tex_format, tex_type, GL_NEAREST, false );
    //CreateDataTexture("render_texture", resX, resY, tex_format, tex_type, GL_TEXTURE_2D);
    //create texture - for bloom effect
	m_texture_cache->Create2DManual( "bloom_texture", resX, resY, tex_format, tex_type, GL_NEAREST, false );
    //CreateDataTexture("bloom_texture", resX, resY, tex_format, tex_type, GL_TEXTURE_2D);
    //create texture - for blur
	m_texture_cache->Create2DManual( "blur_texture", resX, resY, tex_format, tex_type, GL_NEAREST, false );
    //CreateDataTexture("blur_texture", resX, resY, tex_format, tex_type, GL_TEXTURE_2D);
    //create texture - for store normal values
    if(m_useNormalBuffer)
		m_texture_cache->Create2DManual( "normal_texture", resX, resY, tex_format, tex_type, GL_NEAREST, false );
        //CreateDataTexture("normal_texture", resX, resY, tex_format, tex_type, GL_TEXTURE_2D);

    //create renderbuffers
    glGenRenderbuffers(1, &m_r_buffer_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,resX, resY);

    //if MSAA enabled, create also multisampled framebuffer (only when platform supports it)
    if(m_msamples > 1 && GLEW_EXT_framebuffer_multisample)
    {
        //Create multisampled color renderbuffer
        glGenRenderbuffers(1, &m_r_buffer_colorMSAA);
        glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_colorMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msamples, tex_format, resX, resY);

        //Create multisampled normal renderbuffer
        if(m_useNormalBuffer)
        {
            glGenRenderbuffers(1, &m_r_buffer_normalMSAA);
            glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_normalMSAA);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msamples, tex_format, resX, resY);
        }

        //Create multisampled depth renderbuffer
        glGenRenderbuffers(1, &m_r_buffer_depthMSAA);
        glBindRenderbuffer(GL_RENDERBUFFER, m_r_buffer_depthMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msamples, GL_DEPTH_COMPONENT,resX, resY);

        //Attach to multisampled framebuffer
        glGenFramebuffers(1, &m_f_bufferMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, m_f_bufferMSAA);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_r_buffer_colorMSAA);
        if(m_useNormalBuffer)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, m_r_buffer_normalMSAA);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_r_buffer_depthMSAA);

        //check FBO creation
        if(!CheckFBO())
        {
            cout<<"WARNING: Multisampled FBO creation failed, falling back to regular FBO.\n";
            m_msamples = 1;
        }
    }

    //Finally, create framebuffer
    glGenFramebuffers(1, &m_f_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_f_buffer);

    //attach texture to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["render_texture"], 0);
    //attach also normal texture if desired (e.g. for SSAO)
    if(m_useNormalBuffer)
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, m_tex_cache["normal_texture"], 0);
    //attach render buffers
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_r_buffer_depth);


    //check FBO creation
    if(!CheckFBO())
    {
        m_useHDR = m_useSSAO = m_useNormalBuffer = false;
        throw ERR;
    }

    // Go back to regular frame buffer rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //update load list
    UpdateLoadList(34);

    //HDR passes - render to texture, bloom effect, SSAO and tonemapping
    AddMaterial("mat_bloom_hdr_ssao",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
    
    //various defines depending if we want HDR only, SSAO only or both together
    string defines = "#define BLOOM\n";
    if(m_useSSAO)       //SSAO only
    {
        defines = "#define SSAO\n";        
        AddTexture("mat_bloom_hdr_ssao","normal_texture",RENDER_TEXTURE);   //add normal texture
        if(m_useHDR)    //HDR + SSAO
            defines = "#define BLOOM\n#define SSAO\n";
    }
    AddTexture("mat_bloom_hdr_ssao","render_texture",RENDER_TEXTURE);
    AddTexture("mat_bloom_hdr_ssao","data/tex/random.tga");
    CustomShader("mat_bloom_hdr_ssao","data/shaders/quad.vert", "data/shaders/bloom_ssao.frag", " ",defines.c_str());

    //tonemapping
    AddMaterial("mat_tonemap",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
    AddTexture("mat_tonemap","render_texture",RENDER_TEXTURE);
    AddTexture("mat_tonemap","blur_texture",RENDER_TEXTURE);
    if(m_useNormalBuffer)
        AddTexture("mat_tonemap","normal_texture",RENDER_TEXTURE);
    CustomShader("mat_tonemap","data/shaders/quad.vert","data/shaders/tonemap.frag");

    //blur
    AddMaterial("mat_blur_horiz",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
    AddTexture("mat_blur_horiz","bloom_texture",RENDER_TEXTURE);
    CustomShader("mat_blur_horiz","data/shaders/quad.vert","data/shaders/blur.frag", " ","#define HORIZONTAL\n");
    SetUniform("mat_blur_horiz", "texsize", glm::ivec2(resX, resY));       //send texture size info

    AddMaterial("mat_blur_vert",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
    AddTexture("mat_blur_vert","bloom_texture",RENDER_TEXTURE);
    CustomShader("mat_blur_vert","data/shaders/quad.vert","data/shaders/blur.frag", " ", "#define VERTICAL\n");
    SetUniform("mat_blur_vert", "texsize", glm::ivec2(resX, resY));       //send texture size info
}
*/

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
