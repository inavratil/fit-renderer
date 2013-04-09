#include "MipmapPass.h"

//-----------------------------------------------------------------------------

MipmapPass::MipmapPass( GLuint _tex )
{
	//-- assign texture
	assert( _tex != 0 );
	m_tex = _tex;

	//-- setup FBO
	m_fbo = m_FBOManager->CreateAndAttach( m_tex );
}

//-----------------------------------------------------------------------------

MipmapPass::~MipmapPass(void)
{
}

//-----------------------------------------------------------------------------

void MipmapPass::Render()
{
	m_FBOManager->BindBuffer( m_fbo );

	for(int i=1, j = 128.0/2; j>=1; i++, j/=2)
	{
		glViewport(0, 0, j, j);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, i);
		glClear(GL_COLOR_BUFFER_BIT );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, m_tex);

		//SetUniform("mat_aliasMipmap", "offset", 0.5f/((float)j*2.0f));
		//SetUniform("mat_aliasMipmap", "mip_level", i-1);
		//RenderPass("mat_aliasMipmap");

		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	m_FBOManager->UnbindBuffer();

}

//-----------------------------------------------------------------------------
