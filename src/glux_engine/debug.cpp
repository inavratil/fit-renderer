#include "scene.h"

//-- Vola se na konci funkce scene->PostInit()
bool TScene::InitDebug()
{
	{
		//shader showing shadow map alias error
		//AddMaterial("mat_aliasError");
		//AddTexture("mat_aliasError", "data/tex/error_color.tga", RENDER_TEXTURE);
		//CustomShader("mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag");

		GLuint buffer, fbo, tex;

		//TextureCache::Instance()->Add("aliaserr_texture", 128.0, 128.0, GL_RGBA16F, GL_FLOAT);
		//TextureCache::Instance()->Add("aliaserr_texture2", 128.0, 128.0, GL_RGBA16F, GL_FLOAT);
		
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (int face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, 128.0, 128.0, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
		m_tex_cache["aliaserr_texture_cube_color"] = tex;

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		

		//create renderbuffers
		glGenRenderbuffers(1, &buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,128.0, 128.0);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//attach texture to the frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_tex_cache["aliaserr_texture_cube_color"], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, buffer);

		FBOManager::Instance()->Add("dbg_aliaserr_color", fbo);

		//check FBO creation
		if(!CheckFBO())
		{
			ShowMessage("ERROR: FBO creation for dbg_aliaserr failed!",false);
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		GLuint buffer, fbo, tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 64.0, 64.0, 0, GL_RGBA, GL_FLOAT, 0);

		m_tex_cache["tex_random_vpls"] = tex;

		AddMaterial("mat_randomVPLs",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
		AddTexture("mat_randomVPLs","data/tex/random.tga");
		CustomShader("mat_randomVPLs", "data/shaders/quad.vert", "data/shaders/random_vpls.frag");

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//attach texture to the frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["tex_random_vpls"], 0);
		
		FBOManager::Instance()->Add("fbo_random_vpls", fbo);

		//check FBO creation
		if(!CheckFBO())
		{
			ShowMessage("ERROR: FBO creation for fbo_random_vpls failed!",false);
			return false;
		}

		// Go back to regular frame buffer rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error

	{
		//shader showing shadow map alias error
		AddMaterial("mat_aliasError");
		AddTexture("mat_aliasError", "data/tex/error_color.tga", RENDER_TEXTURE);
		CustomShader("mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag");

		GLuint buffer, fbo, tex;

		//TextureCache::Instance()->Add("aliaserr_texture", 128.0, 128.0, GL_RGBA16F, GL_FLOAT);
		//TextureCache::Instance()->Add("aliaserr_texture2", 128.0, 128.0, GL_RGBA16F, GL_FLOAT);
		
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (int face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA16F, 128.0, 128.0, 0, GL_RGBA, GL_FLOAT, 0);
		}
		m_tex_cache["aliaserr_texture_cube_pos"] = tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (int face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA16F, 128.0, 128.0, 0, GL_RGBA, GL_FLOAT, 0);
		}
		m_tex_cache["aliaserr_texture_cube_norm"] = tex;

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		

		//create renderbuffers
		glGenRenderbuffers(1, &buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,128.0, 128.0);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//attach texture to the frame buffer
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"), 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture2"), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_tex_cache["aliaserr_texture_cube_pos"], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_tex_cache["aliaserr_texture_cube_norm"], 0);
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
	GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error

	glEnable(GL_DEPTH_TEST);

	if(m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOManager::Instance()->Get("dbg_aliaserr") );
	glDrawBuffers(2, mrt);
	glViewport( 0, 0, 128.0, 128.0 );

	glm::vec2 rotArr[6] = {
		glm::vec2(0.0, -270.0),
		glm::vec2(0.0, -90.0),
		glm::vec2(-90.0, -270.0),
		glm::vec2(90.0, -270.0),
		glm::vec2(0.0, -180.0),
		glm::vec2(0.0, 0.0)	
	};
	glm::vec3 old_rot = GetCameraRot();
	for(int f=0; f<6; ++f)
	{
		RotateCameraAbs(rotArr[f].x, A_X);
        RotateCameraAbs(rotArr[f].y, A_Y);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+f, m_tex_cache["aliaserr_texture_cube_pos"], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X+f, m_tex_cache["aliaserr_texture_cube_norm"], 0);

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

				m_io->second->Draw( m_materials["mat_aliasError"]->IsTessellated() );
			}
		}
	}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 	FBOManager::Instance()->Get("dbg_aliaserr_color"));
	glDrawBuffers(1, mrt);

	for(int f=0; f<6; ++f)
	{
		RotateCameraAbs(rotArr[f].x, A_X);
        RotateCameraAbs(rotArr[f].y, A_Y);

		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+f, m_tex_cache["aliaserr_texture_cube_color"], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//render all opaque objects
		DrawScene(DRAW_OPAQUE);

		//then transparent objects
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		DrawScene(DRAW_TRANSPARENT);
		glDisable(GL_BLEND);

	}

	RotateCameraAbs(old_rot.x, A_X);
    RotateCameraAbs(old_rot.y, A_Y);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glBindFramebuffer( GL_FRAMEBUFFER, FBOManager::Instance()->Get("fbo_random_vpls") );
	glDrawBuffers(1, mrt);
	glViewport(0,0,64,64);
	glClear(GL_COLOR_BUFFER_BIT);

	SetUniform("mat_randomVPLs", "seed", (float) (rand()%100)  );
	RenderPass("mat_randomVPLs");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}