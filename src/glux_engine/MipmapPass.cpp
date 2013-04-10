#include "MipmapPass.h"

//-----------------------------------------------------------------------------

MipmapPass::MipmapPass( GLuint _tex ) :
	Pass( _tex )
{
}

//-----------------------------------------------------------------------------

MipmapPass::~MipmapPass(void)
{
}

//-----------------------------------------------------------------------------

void MipmapPass::Render()
{

	for(int i=1, j = 128.0/2; j>=1; i++, j/=2)
	{
		glViewport(0, 0, j, j);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  m_output_textures[0].id , i);
		glClear(GL_COLOR_BUFFER_BIT );


		//SetUniform("mat_aliasMipmap", "offset", 0.5f/((float)j*2.0f));
		//SetUniform("mat_aliasMipmap", "mip_level", i-1);
		//RenderPass("mat_aliasMipmap");

	}
}

//-----------------------------------------------------------------------------

void MipmapPass::Activate()
{
	Pass::Activate();

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D,  m_output_textures[0].id );
}

//-----------------------------------------------------------------------------

void MipmapPass::Deactivate()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	Pass::Deactivate();
}

//-----------------------------------------------------------------------------