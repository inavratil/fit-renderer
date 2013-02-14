#include "scene.h"

//-- Vola se na konci funkce scene->PostInit()
bool TScene::InitDebug()
{
	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error

	{
		//shader showing shadow map alias error
		AddMaterial("mat_aliasError");
		AddTexture("mat_aliasError", "data/tex/error_color.tga", RENDER_TEXTURE);
		CustomShader("mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag");

		GLuint buffer, fbo;

		TextureCache::Instance()->Add("aliaserr_texture", m_resx, m_resy, GL_RGBA16F, GL_FLOAT);
		
		//create renderbuffers
		glGenRenderbuffers(1, &buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,m_resx, m_resy);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//attach texture to the frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"), 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, buffer);

		FBOManager::Instance()->Add("dbg_aliaserr", fbo);

		//check FBO creation
		if(!CheckFBO())
		{
			ShowMessage("ERROR: FBO creation for dbg_aliaserr failed!",false);
			return false;
		}

		// Go back to regular frame buffer rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_aerr_buffer = new float[m_resx*m_resy];
	}

	return true;
}

void TScene::RenderDebug()
{
	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error

	glEnable(GL_DEPTH_TEST);

	if(m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOManager::Instance()->Get("dbg_aliaserr") );
	glViewport( 0, 0, m_resx, m_resy );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//then other with depth-only shader
	m_materials["mat_aliasError"]->RenderMaterial();
	
	for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
	{
		if( m_io->second->GetSceneID() == m_sceneID )
		{
			//update matrix
			glm::mat4 m = m_viewMatrix * m_io->second->GetMatrix();
			SetUniform("mat_aliasError", "in_ModelViewMatrix", m);

			//TODO: hack na zobrazeni kamery
			if(m_io->first == "camera")
				SetUniform("mat_aliasError", "is_camera",1);
			else
				SetUniform("mat_aliasError", "is_camera",0);

			m_io->second->Draw( m_materials["mat_aliasError"]->IsTessellated() );
		}
	}


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}