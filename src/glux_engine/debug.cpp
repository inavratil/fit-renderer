#include "scene.h"

//-- Vola se na konci funkce scene->PostInit()
bool TScene::InitDebug()
{
	GLuint buffer, fbo, tex;

	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error

	{
		//shader showing shadow map alias error
		AddMaterial("mat_aliasError");
		AddTexture("mat_aliasError", "data/tex/error_color.tga", RENDER_TEXTURE);
		CustomShader("mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag");

		TextureCache::Instance()->Add("aliaserr_texture", 128.0, 128.0, GL_RGBA16F, GL_FLOAT);
		
		//create renderbuffers
		glGenRenderbuffers(1, &buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,m_resx, m_resy);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//attach texture to the frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"), 0);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, buffer);

		FBOManager::Instance()->Add("dbg_aliaserr", fbo);

		//check FBO creation
		if(!CheckFBO())
		{
			ShowMessage("ERROR: FBO creation for dbg_aliaserr failed!",false);
			return false;
		}

		// Go back to regular frame buffer rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	///////////////////////////////////////////////////////////////////////////////
	//-- Mipmaps

	{
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 128.0, 128.0, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGenerateMipmap( GL_TEXTURE_2D );

		TextureCache::Instance()->Add("aliaserr_mipmap", tex );
		

	}

	return true;
}

void TScene::RenderDebug()
{
	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error
	glGetError();
	glEnable(GL_DEPTH_TEST);

	if(m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOManager::Instance()->Get("dbg_aliaserr") );
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"), 0);
	glViewport( 0, 0, 128.0, 128.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//then other with depth-only shader
	m_materials["mat_aliasError"]->RenderMaterial();
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"] );
	SetUniform("mat_aliasError", "MTEX_2Dfunc_values", 1 );
	
	for(m_io = m_objects.begin(); m_io != m_objects.end(); ++m_io)
	{
		if( m_io->second->GetSceneID() == m_sceneID )
		{
			//update matrix
			glm::mat4 m = m_viewMatrix * m_io->second->GetMatrix();
			SetUniform("mat_aliasError", "in_ModelViewMatrix", m);

			m_io->second->Draw( m_materials["mat_aliasError"]->IsTessellated() );
		}
	}

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	/*	
	float *aerr_buffer = new float[128*128];
	glBindTexture(GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"));
	glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, aerr_buffer);

	float sum = 0.0;
	int count = 0;
	for( int i=0; i<(128*128); ++i )
	{
		if( aerr_buffer[i] > 0.0 )
		{
			sum += aerr_buffer[i];
			count++;
		}
	}

	delete aerr_buffer;

	cout << sum/count << " ";
	*/
}