#include "DPShadowMap.h"

#include "sdk/SimplePass.h"
#include "sdk/ScreenSpaceMaterial.h"
#include "shaderGen/SFDPShadowMap.h"

//-----------------------------------------------------------------------------

DPShadowMap::DPShadowMap( TScene* _scene ) :
IShadowTechnique( _scene )
{
	_Init();
}

//-----------------------------------------------------------------------------

DPShadowMap::~DPShadowMap(void)
{
	_Destroy();
}

//-----------------------------------------------------------------------------

void DPShadowMap::_Init()
{
	//-- shadow map parameters
	m_i_shadow_res = 1024;
	m_f_intensity = 0.1f;
	m_b_use_pcf = false;
	m_b_draw_shadow_map = false;
	m_b_draw_alias_error = false;

	//-- setup shader feature
	m_pShaderFeature = new SFDPShadowMap();
}

//-----------------------------------------------------------------------------

void DPShadowMap::_Destroy()
{
	if( m_pShaderFeature )
		delete m_pShaderFeature;
}

//-----------------------------------------------------------------------------

bool DPShadowMap::Initialize()
{
	//-- The light must be set
	if( !m_pLight ) return false;

	try
	{
		//-- Get access to managers and caches
		TextureCachePtr texture_cache = m_scene->GetTextureCache();
		MaterialManagerPtr material_manager = m_scene->GetMaterialManager();

		int output_size = m_i_shadow_res;

		TexturePtr tex_shadow = texture_cache->Create2DArrayManual( 
			"tex_omni_shadowmap",
			output_size, output_size, 
			2,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			GL_NEAREST,
			false
			);
		tex_shadow->SetType( CUSTOM );
		tex_shadow->SetIntensity( m_f_intensity );

		//-- pass
		SimplePassPtr pass_omni_depth = new SimplePass( output_size, output_size );
		pass_omni_depth->AttachOutputTexture( 0, tex_shadow, true );
		this->AppendPass("pass_omni_depth", pass_omni_depth );	

		//FIXME: zapisujeme pouze do depth textury
		//glDrawBuffer(GL_NONE); 
		//glReadBuffer(GL_NONE);

		for(material_manager->Begin(); 
			!material_manager->End();
			material_manager->Next())
		{
			Material* mat = material_manager->GetItem();
			if( mat->IsScreenSpace() || !mat->IsReceivingShadow() ) continue;
			if (mat->GetSceneID() == m_scene->GetSceneID() )
			{
				mat->AddTexture( tex_shadow, "tex_shadowmap" );
				static_cast<GeometryMaterial*>(mat)->AddFeature( this->GetShaderFeature() );
			}
		}

		if( m_b_use_pcf )
			m_pShaderFeature->AddVariable( "USE_PCF", "", ShaderFeature::DEFINE );


		//-- shader showing shadow map alias error
		ScreenSpaceMaterial* mat_aliasError = new ScreenSpaceMaterial( "mat_aliasError", "data/shaders/default_shadow_alias_error.vert", "data/shaders/default_shadow_alias_error.frag" );
		mat_aliasError->AddTexture( texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "mat_aliasErrorBaseA" );
		material_manager->AddMaterial( mat_aliasError );
	}
	catch( int )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

void DPShadowMap::PreRender()
{
	//-----------------------------------------------------------------------------
	//-- Get access to managers and caches
	MaterialManagerPtr material_manager = m_scene->GetMaterialManager();
	TextureCachePtr texture_cache = m_scene->GetTextureCache();

	//-----------------------------------------------------------------------------
	//-- Initialize local variables

	glm::mat4 lightViewMatrix[2];

	//-- position of camera in eye space = 0.0
	//glm::vec3 cam_pos = glm::vec3(0.0);

	_PreDrawDepthMap();


	//-----------------------------------------------------------------------------
	///draw only back faces of polygons
	glCullFace(GL_FRONT);
	//glColorMask(0, 0, 0, 0);      //disable colorbuffer write
	glEnable(GL_CLIP_PLANE0);

	if(m_scene->IsWireframe())
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//-- Activate pass, i.e. it binds the associated FBO
	this->GetPassPtr("pass_omni_depth")->Activate();

	//TWO PASS - FRONT AND BACK
	for(int i=0; i<2; i++)
	{
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_cache->Get("tex_omni_shadowmap"), 0, i);               

		glClear( GL_DEPTH_BUFFER_BIT );

		lightViewMatrix[i] =_GetLightViewMatrix( i );
		_UpdateShaderUniforms( i );

		m_scene->DrawGeometry("_mat_default_shadow_omni", lightViewMatrix[i]);

	}

	//-- Deactivate pass, i.e. it unbinds the associated FBO
	this->GetPassPtr("pass_omni_depth")->Deactivate();


	if( !m_scene->IsWireframe() )
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	//Finish, restore values
	glDisable( GL_CLIP_PLANE0 );
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);

	//-----------------------------------------------------------------------------
	//-- set light matrices and near/far planes to all materials
	for(material_manager->Begin(); 
		!material_manager->End();
		material_manager->Next())
	{
		Material* mat = material_manager->GetItem();
		mat->SetUniform("lightModelView[0]", lightViewMatrix[0]);
		mat->SetUniform("lightModelView[1]", lightViewMatrix[1]);
		mat->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
	}

	MaterialPtr mat_aliasError = material_manager->GetMaterial( "mat_aliasError" );
	mat_aliasError->SetUniform("lightModelView[0]", lightViewMatrix[0]);
	mat_aliasError->SetUniform("lightModelView[1]", lightViewMatrix[1]);
	mat_aliasError->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
}

//-----------------------------------------------------------------------------

void DPShadowMap::_PreDrawDepthMap()
{
}

//-----------------------------------------------------------------------------

glm::mat4 DPShadowMap::_GetLightViewMatrix( int _i )
{
	float z_direction = 1.0;
	if(_i == 1)
		z_direction = -1.0;  

	return glm::lookAt(
		m_pLight->GetPos(), 
		m_pLight->GetPos() + glm::vec3(-z_direction*m_scene->GetCameraPtr()->GetFarPlane(), 0.0f, 0.0f ), 
		glm::vec3(0.0f, 1.0f, 0.0f) );
}

//-----------------------------------------------------------------------------

void DPShadowMap::_UpdateShaderUniforms( int _i )
{
	Material* _mat_default_shadow_omni = m_scene->GetMaterialManager()->GetMaterial( "_mat_default_shadow_omni" );
	_mat_default_shadow_omni->SetUniform("near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));
}

//-----------------------------------------------------------------------------

void DPShadowMap::PostRender()
{
	if( m_b_draw_alias_error )
		m_scene->DrawGeometry( "mat_aliasError", m_scene->GetViewMatrix() );
}

//-----------------------------------------------------------------------------

void DPShadowMap::SetShadowParams( int _res, int _intensity )
{
	m_i_shadow_res = _res;
	m_f_intensity = _intensity;
}

//-----------------------------------------------------------------------------
///@brief Toggle shadow map drawing
void DPShadowMap::SetDrawShadowMap(bool _flag)
{
	m_b_draw_shadow_map = _flag;
}

//-----------------------------------------------------------------------------

void DPShadowMap::SetDrawAliasError(bool _flag)
{
	m_b_draw_alias_error = _flag;
}
//-----------------------------------------------------------------------------
void DPShadowMap::_EvaluateBestConfiguration()
{
	//#define TEST_DPSM
#if 0
	glm::vec2 res_cut_angle;
	glm::vec2 res_parab_angle;

	float min_sum = 1000.0f;

	static bool first = true;
	if( first )
	{
#ifdef TEST_DPSM
		float d_cutx = 0.0;
		float d_cuty = 0.0;
#else
		for( float d_cuty = -64.0; d_cuty < 64.0; d_cuty += 10 )
			for( float d_cutx = -64.0; d_cutx < 64.0; d_cutx += 10 )
#endif
			{
				cout << "Cut angles:" << d_cutx << "-" << d_cuty << endl;
				for( float d_roty = 0; d_roty <= 180; d_roty += 18 )
				{
					for( float d_rotx = 0; d_rotx <= 180; d_rotx += 18 )
					{
						cut_angle.x = d_cutx;
						cut_angle.y = d_cuty;
						parab_angle.x = d_rotx;
						parab_angle.y = d_roty;
						//cut_tgangle = -1.5;
						//parab_angle.x = 54;
						//parab_angle.y = 162;
						///draw all lights
						unsigned i;
						for(i=0, il = lights.begin(); il != lights.end(), i<lights.size(); il++, i++)
						{
							///if light has a shadow, render scene from light view to texture (TScene::RenderShadowMap())
							if(il->HasShadow())
							{

								//render shadow map
								if(il->GetType() == OMNI)
									RenderShadowMapOmni(&(*il));
								else 
									RenderShadowMap(&(*il));
							}
						}

						RenderAliasError();
						glBindTexture(GL_TEXTURE_2D, tex_cache["aliaserr_texture"]);
						glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, aerr_buffer);

						float sum = 0.0;
						int count = 0;
						for( int i=0; i<(resx*resy); ++i )
						{
							if( aerr_buffer[i] > 0.0 )
							{
								sum += aerr_buffer[i];
								count++;
							}
						}

						//cout << ""<< sum/count << ";" << cut_angle.x << ";" << parab_angle.x << ";" << parab_angle.y << endl;
						cout << sum/count << " ";

						if( sum/count < min_sum )
						{
							min_sum = sum/count;
							res_cut_angle.x = cut_angle.x;
							res_cut_angle.y = cut_angle.y;
							res_parab_angle.x = parab_angle.x;
							res_parab_angle.y = parab_angle.y;
						}
					}
					cout << endl;
				}
			}

			cout << "Res: "<< min_sum << ", " << res_cut_angle.x << ", " << res_cut_angle.y << ", " << res_parab_angle.x << ", " << res_parab_angle.y << endl;
			first = false;
	} //-- first
#endif

}

//-----------------------------------------------------------------------------