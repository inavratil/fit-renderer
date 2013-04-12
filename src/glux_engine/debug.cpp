#include "scene.h"
#include "SimplePass.h"

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

		SimplePass* pass_eyespace_aliaserror = new SimplePass( 128, 128 );
		pass_eyespace_aliaserror->AttachOutputTexture( 0, tex_aliaserr );
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
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"] );
	SetUniform("mat_aliasError", "MTEX_2Dfunc_values", 1 );
	
	DrawGeometry( "mat_aliasError", m_viewMatrix );

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