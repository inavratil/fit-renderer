#include "scene.h"
#include "sdk/SimplePass.h"

//-- Vola se na konci funkce scene->PostInit()
bool TScene::InitDebug()
{
	///////////////////////////////////////////////////////////////////////////////
	//-- Render Alias error

	{
		//-- texture
		m_texture_cache->Create2DManual("aliaserr_texture",
			128.0, 128.0,	//-- width and height
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			false			//-- mipmap generation
			);
		//-- pass
		SimplePass* pass_eyespace_aliaserror = new SimplePass( 128, 128 );
		pass_eyespace_aliaserror->AttachOutputTexture( 0, m_texture_cache->GetPtr( "aliaserr_texture" ) );
		AppendPass( "pass_eyespace_aliaserror", pass_eyespace_aliaserror );
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

	m_passes["pass_eyespace_aliaserror"]->Activate();
	
	DrawGeometry( "mat_perspective_error", m_viewMatrix );

	m_passes["pass_eyespace_aliaserror"]->Deactivate();
    
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