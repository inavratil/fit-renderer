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

		//-- texture
		GLuint tex_aliaserr = m_texture_cache->Create2DManual("aliaserr_texture",
			128.0, 128.0,	//-- width and height
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			false			//-- mipmap generation
			);
		//-- FBO
		FBOPtr fbo_aliaserr = m_FBOManager->CreateFBO( 128, 128, "dbg_aliaserr" );
		fbo_aliaserr->AttachColorTexture( tex_aliaserr );
	}

	///////////////////////////////////////////////////////////////////////////////
	//-- Mipmaps

	{

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

	m_FBOManager->Get("dbg_aliaserr")->Bind();
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