#include "SplineWarpedShadow.h"

#include "../precomputed_diffs.h"

#include "shadow.h"
#include "shaderGen/SFSplineWarpedShadow.h"
#include "resources/TextureCache.h"
#include "scene.h"
#include "sdk/SimplePass.h"

//----------------------------------------------------------------------------

SplineWarpedShadow::SplineWarpedShadow( TScene* _scene ) :
	IWarpedShadowTechnique( _scene )
{
	_Init();
}

//----------------------------------------------------------------------------

SplineWarpedShadow::~SplineWarpedShadow(void)
{
}

//----------------------------------------------------------------------------

void SplineWarpedShadow::_Init()
{
	m_sName = "Spline";
	m_sDefines = "#define SPLINE_WARP\n";

	m_pFuncValues = NULL;

	m_pShaderFeature = new SFSplineWarpedShadow();

	Mcr = 0.5 * glm::mat4(
		 0.0f,	2.0f,	0.0f,	0.0f,
		-1.0f,	0.0f,	1.0f,	0.0f,
		 2.0f, -5.0f,	4.0f,  -1.0f,
		-1.0f,	3.0f,	-3.0f,  1.0f
		);
}

//----------------------------------------------------------------------------

bool SplineWarpedShadow::Initialize()
{
	//-- we don't associate light with the technique or the technique is not used in the scene
	if ( !m_pLight || !m_scene ) 
		return false;

	int sh_res = m_pLight->ShadowSize();

	try
	{
		//-----------------------------------------------------------------------------
		//-- create data textures

		TextureCache::Instance()->Create2DManual( "tex_camAndLightCoords", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		TextureCache::Instance()->Create2DManual( "tex_stencil_color", 128, 128, GL_RGBA16F,	GL_FLOAT, GL_NEAREST, false );
		TextureCache::Instance()->Create2DManual( "tex_output", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		TextureCache::Instance()->Create2DManual( "MTEX_ping", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		TextureCache::Instance()->Create2DManual( "MTEX_pong", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		//FIXME: precision??? nestaci 16F ?
		TextureCache::Instance()->Create2DManual( "MTEX_2Dfunc_values", this->GetControlPointsCount(), this->GetControlPointsCount(), GL_RGBA32F, GL_FLOAT, GL_NEAREST, false );
		TextureCache::Instance()->Create2DManual( "MTEX_2Dfunc_values_ping", this->GetControlPointsCount(), this->GetControlPointsCount(), GL_RGBA32F, GL_FLOAT, GL_NEAREST, false );
		
		TexturePtr tex_shadow = TextureCache::Instance()->Create2DArrayManual("tex_shadow",
			sh_res, sh_res,			//-- width and height
			2,						//-- number of layers
			GL_DEPTH_COMPONENT,		//-- internal format
			GL_FLOAT,				//-- type of the date
			GL_NEAREST,				//-- filtering
			false					//-- mipmap generation
			);
		tex_shadow->SetType( SHADOW_OMNI );
		tex_shadow->SetIntensity( m_pLight->ShadowIntensity() );
		TextureCache::Instance()->Create2DArrayManual("MTEX_warped_depth_color",
			sh_res, sh_res,	//-- width and height
			2,				//-- number of layers
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			false			//-- mipmap generation
			);
		TextureCache::Instance()->Create2DManual("aliaserr_mipmap",
			128, 128,		//-- width and height
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			true			//-- mipmap generation
			);
		
		//-----------------------------------------------------------------------------
		//-- cam coords
		{
			//-- shader
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_camAndLightCoords_afterDP","data/shaders/warping/camAndLightCoords_afterDP.vert", "data/shaders/warping/camAndLightCoords_afterDP.frag" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_2Dfunc_values" ), "funcTex" );
			m_scene->AddMaterial( mat );

			//-- pass
			SimplePassPtr pass_coords = new SimplePass( sh_res/8, sh_res/8 );
			pass_coords->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "tex_camAndLightCoords" ) );
			pass_coords->AttachOutputTexture( 1, TextureCache::Instance()->GetPtr( "tex_stencil_color" ) );
			pass_coords->SetShader( mat );
			m_scene->AppendPass("pass_coords", pass_coords );
		}

		//-----------------------------------------------------------------------------
		//-- output
		{				
			//-- shader
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_compute_aliasError", "data/shaders/warping/computeAliasError.vert"/*quad.vert*/, "data/shaders/warping/computeAliasError.frag" );
			mat->AddTexture( TextureCache::Instance()->CreateFromImage( "data/tex/error_color.tga" ), "tex_error" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "tex_camAndLightCoords" ), "tex_coords" );
			m_scene->AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_compute_aliasError = new SimplePass( sh_res/8, sh_res/8 );
			pass_compute_aliasError->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "tex_output" ) );
			pass_compute_aliasError->DisableDepthBuffer();
			pass_compute_aliasError->SetShader( mat );
			m_scene->AppendPass("pass_compute_aliasError", pass_compute_aliasError );
		}
		//-----------------------------------------------------------------------------

		//FIXME: Tohle by melo prijit do Init metody dane shadow techniky
		this->GetShaderFeature()->AddTexture( "MTEX_2Dfunc_values", TextureCache::Instance()->Get("MTEX_2Dfunc_values"), 1.0, ShaderFeature::FS );

		//FIXME: to shadowID by se mohlo/melo nastavovat jinde
		m_pLight->SetShadowTexID( TextureCache::Instance()->Get( "tex_shadow" ) );

		//-----------------------------------------------------------------------------
		//blur
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasblur_horiz","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ","#define HORIZONTAL\n" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "tex_output" ), "bloom_texture" );
			m_scene->AddMaterial( mat );

			//-- pass
			SimplePassPtr pass_horiz_blur = new SimplePass( sh_res/8, sh_res/8 );
			pass_horiz_blur->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "MTEX_ping" ) );
			pass_horiz_blur->DisableDepthBuffer();
			m_scene->AppendPass("pass_horiz_blur", pass_horiz_blur );
		}
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasblur_vert","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ", "#define VERTICAL\n" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_ping" ), "bloom_texture" );
			m_scene->AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_vert_blur = new SimplePass( sh_res/8, sh_res/8 );
			pass_vert_blur->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "MTEX_pong" ) );
			pass_vert_blur->DisableDepthBuffer();
			m_scene->AppendPass("pass_vert_blur", pass_vert_blur );
		}
		//-----------------------------------------------------------------------------
		//alias gradient
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasgradient","data/shaders/quad.vert","data/shaders/warping/aliasgradient.frag" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_pong" ), "tex_blurred_error" );
			m_scene->AddMaterial( mat );	
			//-- pass
			SimplePassPtr pass_gradient = new SimplePass( sh_res/8, sh_res/8 );
			pass_gradient->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "MTEX_ping" ) );
			pass_gradient->DisableDepthBuffer();
			m_scene->AppendPass("pass_gradient", pass_gradient );
		}
		//-----------------------------------------------------------------------------
		//2D func values
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_get_2Dfunc_values","data/shaders/quad.vert","data/shaders/warping/get2DfuncValues.frag" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_ping" ), "gradient_map" );
			m_scene->AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_func_values = new SimplePass( this->GetControlPointsCount(), this->GetControlPointsCount() );
			pass_func_values->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "MTEX_2Dfunc_values" ) );
			pass_func_values->DisableDepthBuffer();
			m_scene->AppendPass("pass_func_values", pass_func_values );
		}
		//-----------------------------------------------------------------------------
		//draw depth with warping
		{
			ScreenSpaceMaterial* mat = 
				new ScreenSpaceMaterial( 
				"mat_depth_with_warping",
				"data/shaders/warping/drawDepthWithWarping.vert", "data/shaders/warping/drawDepthWithWarping.frag", 
				this->GetDefines(), "" 
				);
			mat->AddTexture( TextureCache::Instance()->GetPtr("MTEX_2Dfunc_values"), "funcTex" );
			m_scene->AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_warped_depth = new SimplePass( sh_res, sh_res );
			pass_warped_depth->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr("MTEX_warped_depth_color") );
			pass_warped_depth->AttachOutputTexture( 0, TextureCache::Instance()->GetPtr( "tex_shadow" ), true );
			m_scene->AppendPass("pass_warped_depth", pass_warped_depth );	
		}
		//-----------------------------------------------------------------------------
		//-- blit pass
		//BlitPass *bp = new BlitPass( 128, 128 );
		//bp->AttachReadTexture( TextureCache::Instance()->GetPtr( "tex_output" ) );
		//bp->AttachDrawTexture( TextureCache::Instance()->GetPtr( "aliaserr_mipmap" ) );
		//AppendPass("pass_blit_0", bp );
		//-- copy squared error
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_copy_squared_error", "data/shaders/quad.vert", "data/shaders/warping/copy_squared_error.frag" );
			mat->AddTexture( TextureCache::Instance()->GetPtr("tex_output"), "tex_alias_error" );
			m_scene->AddMaterial( mat );

			//-- pass
			SimplePass *mp = new SimplePass( 128, 128 );
			mp->AttachOutputTexture(0, TextureCache::Instance()->GetPtr("aliaserr_mipmap") ); 
			mp->DisableDepthBuffer();
			mp->SetShader( mat );
			m_scene->AppendPass("pass_copy_squared_error", mp);
		}
		//-----------------------------------------------------------------------------
		//-- mipmap pass
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasMipmap", "data/shaders/quad.vert", "data/shaders/shadow_alias_mipmap.frag" );
			mat->AddTexture( TextureCache::Instance()->GetPtr("aliaserr_mipmap"), "mat_aliasError" );
			m_scene->AddMaterial( mat );

			//-- pass
			SimplePass *mp = new SimplePass( 128, 128 );
			mp->AttachOutputTexture(0, TextureCache::Instance()->GetPtr("aliaserr_mipmap") ); 
			mp->DisableDepthBuffer();
			m_scene->AppendPass("pass_alias_mipmap", mp);
		}
		//-----------------------------------------------------------------------------
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "show_depth_omni", "data/shaders/showDepth.vert", "data/shaders/showDepth_omni.frag" ); 
			mat->AddTexture( TextureCache::Instance()->GetPtr("MTEX_warped_depth_color"), "show_depth_dpShadowA" );
			m_scene->AddMaterial( mat );
		}
		//-----------------------------------------------------------------------------
		{
			//-- shader showing shadow map alias error
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag" );
			mat->AddTexture( TextureCache::Instance()->CreateFromImage( "data/tex/error_color.tga" ), "mat_aliasErrorBaseA" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_2Dfunc_values" ), "MTEX_2Dfunc_values" );
			m_scene->AddMaterial( mat );
		}
		{
			//-- shader showing perspective alias error
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_perspective_error", "data/shaders/perspective_alias_error.vert", "data/shaders/perspective_alias_error.frag" );
			mat->AddTexture( TextureCache::Instance()->CreateFromImage( "data/tex/error_color.tga" ), "tex_error_color" );
			mat->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_2Dfunc_values" ), "tex_2Dfunc_values" );
			m_scene->AddMaterial( mat );
		}
	}
	catch( int )
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------

void SplineWarpedShadow::PreRender()
{
	int sh_res = m_pLight->ShadowSize();
	GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glm::mat4 cam_view_matrix = m_scene->GetViewMatrix();
	glm::mat4 cam_proj_matrix = m_scene->GetProjMatrix();

	//set light projection and light view matrix
	glm::mat4 lightProjMatrix = glm::perspective(90.0f, 1.0f, 1.0f, 1000.0f);
	glm::mat4 lightViewMatrix[2];
	lightViewMatrix[1] = glm::lookAt(m_pLight->GetPos(), m_pLight->GetPos() + glm::vec3(-m_scene->GetCameraPtr()->GetFarPlane(), 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );
	//glm::mat4 lightViewMatrix = glm::lookAt(l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );

	//glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjMatrix));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 coeffsX = glm::mat4( 0.0 );
	glm::mat4 coeffsY = glm::mat4( 0.0 );
	glm::vec4 mask_range = glm::vec4( 128.0, 0.0, 128.0, 0.0 );

	glClearColor(99.0, 0.0, 0.0, 0.0);

	///////////////////////////////////////////////////////////////////////////////
	//-- 1. Render scene from light point of view and store per pixel camera-screen 
	//--	coordinates and light-screen coordinates
	//FIXME: Jake je rozliseni vystupu???

	//-----------------------------------------------------------------------------

	//if(m_wireframe)
	//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable( GL_CLIP_PLANE0 );

	//glEnable( GL_CULL_FACE);
	glCullFace(GL_FRONT);

	m_scene->GetPassPtr( "pass_coords" )->Activate();
	MaterialPtr mat_coords = m_scene->GetPassPtr( "pass_coords" )->GetShader();
	mat_coords->SetUniform("cam_mv", cam_view_matrix );
	mat_coords->SetUniform("cam_proj", cam_proj_matrix );
	mat_coords->SetUniform("near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	mat_coords->SetUniform("grid_res", (float) this->GetControlPointsCount() );
	mat_coords->SetUniform("matrix_ortho", glm::ortho(-512.f, 512.f, -512.f, 512.f, 0.1f, 10000.0f) );
	mat_coords->SetUniform("camera_space_light_position", cam_view_matrix * glm::vec4( m_pLight->GetPos(), 1.0 )); 

	m_scene->DrawGeometry(mat_coords->GetName().c_str(), lightViewMatrix[1]);

	m_scene->GetPassPtr( "pass_coords" )->Deactivate();

	//if(!m_wireframe)
	//	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glCullFace(GL_BACK);
	glDisable( GL_CULL_FACE);

	

	//-----------------------------------------------------------------------------

	//FIXME: tenhle kod by mel prijit do PreRender metody, ale zatim neni vyresen pristup globalni pristup k texturam
	/*
	if ( (string) m_shadow_technique->GetName() ==  "Polynomial" )
	{
		float mask_values[128*128];
		glBindTexture(GL_TEXTURE_2D, TextureCache::Instance()->Get( "tex_stencil_color" ) );
		glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, mask_values);

		for(int i=0; i<128; ++i)
			for(int j=0; j<128; ++j)
			{
				float a = mask_values[j + i*128];
				if( a > 0.0 )
				{
					if( j < mask_range.x ) mask_range.x = j; // left border
					if( j > mask_range.y ) mask_range.y = j; // right border
					if( i < mask_range.z ) mask_range.z = i; // bottom border
					if( i > mask_range.w ) mask_range.w = i; // top border
				}

			}

			m_shadow_technique->UpdateGridRange( mask_range );
	}
	*/

	///////////////////////////////////////////////////////////////////////////////
	//-- 2. Compute alias error based on the coordinates data computed in step 1
	//--	Input: tex_camAndLightCoords (error texture)

	//FIXME: opravdu mohu pouzit tex_coords v plnem rozliseni, pri mensim rozliseni se bude brat jenom kazda n-ta hodnota
	//		textureOffset se pousouva v rozliseni textury, takze kdyz je rozliseni 1024, posune se o pixel v tomto rozliseni
	//		pri rendrovani do 128x128 je mi toto chovani ale na prd ?????
	m_scene->GetPassPtr( "pass_compute_aliasError" )->Activate();
	m_scene->GetPassPtr( "pass_compute_aliasError" )->Render();
	m_scene->GetPassPtr( "pass_compute_aliasError" )->Deactivate();	

#ifndef GRADIENT_METHOD
	//calculate custom mipmaps 
	{
		//-----------------------------------------------------------------------------

		//m_passes["pass_blit_0"]->Process();
		m_scene->GetPassPtr( "pass_copy_squared_error" )->Activate();
		m_scene->GetPassPtr( "pass_copy_squared_error" )->Render(); //RenderPass("mat_copy_squared_error");		
		m_scene->GetPassPtr( "pass_copy_squared_error" )->Deactivate();

		//-----------------------------------------------------------------------------

		glm::vec4 clear_color;
		glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
		glClearColor( 1, 1, 1, 1 );

		glColorMask( 0, 0, 0, 0 );	//-- nutno vypnout, aby se neclearovalo FBO
		m_scene->GetPassPtr( "pass_alias_mipmap" )->Activate();
		glColorMask( 1, 1, 1, 1 );	//-- znovu zapnout

		for(int i=1, j = 128.0/2; j>=1; i++, j/=2)
		{
			glViewport(0, 0, j, j);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_mipmap" ), i);
			glClear(GL_COLOR_BUFFER_BIT );

			m_scene->SetUniform("mat_aliasMipmap", "offset", 0.5f/((float)j*2.0f));
			m_scene->SetUniform("mat_aliasMipmap", "mip_level", i-1);
			m_scene->RenderPass("mat_aliasMipmap");				
		}
	
		m_scene->GetPassPtr( "pass_alias_mipmap" )->Deactivate();


		glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

		//-----------------------------------------------------------------------------

		//memset(g_precomputed_diffs, 0, 19*19*sizeof(glm::vec4));
		//ModifyGrid(g_precomputed_diffs);

		glViewport( 0, 0, 128.0, 128.0 ); //restore viewport
	}
#endif


	///////////////////////////////////////////////////////////////////////////////
	//-- 3. Blur the alias error
	//--	input: MTEX_ouput (horiz), MTEX_ping (vert)

	glDisable(GL_DEPTH_TEST);

	float sigma = 2.7;

	m_scene->GetPassPtr( "pass_horiz_blur" )->Activate();

	//GetMaterial( "mat_aliasblur_horiz" )->AddTexture( TextureCache::Instance()->GetPtr( "tex_output" ), "bloom_texture" );
	m_scene->SetUniform("mat_aliasblur_horiz", "texsize", glm::ivec2(sh_res/8, sh_res/8));
	m_scene->SetUniform("mat_aliasblur_horiz", "kernel_size", 9.0);
	m_scene->SetUniform("mat_aliasblur_horiz", "two_sigma_sq", TWOSIGMA2(sigma));
	m_scene->SetUniform("mat_aliasblur_horiz", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	m_scene->RenderPass("mat_aliasblur_horiz");

	m_scene->GetPassPtr( "pass_horiz_blur" )->Deactivate();

	//-----------------------------------------------------------------------------

	m_scene->GetPassPtr( "pass_vert_blur" )->Activate();

	//GetMaterial( "mat_aliasblur_vert" )->AddTexture( TextureCache::Instance()->GetPtr( "MTEX_ping" ), "bloom_texture" );
	m_scene->SetUniform("mat_aliasblur_vert", "texsize", glm::ivec2(sh_res/8, sh_res/8));
	m_scene->SetUniform("mat_aliasblur_vert", "kernel_size", 9.0);
	m_scene->SetUniform("mat_aliasblur_vert", "two_sigma_sq",TWOSIGMA2(sigma));
	m_scene->SetUniform("mat_aliasblur_vert", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	m_scene->RenderPass("mat_aliasblur_vert");

	m_scene->GetPassPtr( "pass_vert_blur" )->Deactivate();

	//GetMaterial( "mat_aliasblur_horiz" )->DeleteTexture( "bloom_texture" );
	//GetMaterial( "mat_aliasblur_vert" )->DeleteTexture( "bloom_texture" );

	///////////////////////////////////////////////////////////////////////////////
	//-- 4. Compute gradient and store the result per-pixel into 128x128 texture

	glm::vec2 limit = this->GetGrid()->GetOffset() / 128.0f;
	limit /= POLY_BIAS;

	m_scene->GetPassPtr( "pass_gradient" )->Activate();

	//FIXME: limit pocitat nejak inteligentne bez bulharske konstanty
	//SetUniform("mat_aliasgradient", "limit", glm::vec2(100.0f));
	m_scene->SetUniform("mat_aliasgradient", "limit", limit);
	m_scene->RenderPass("mat_aliasgradient");

	m_scene->GetPassPtr( "pass_gradient" )->Deactivate();

	///////////////////////////////////////////////////////////////////////////////
	//-- 5. get a function value from gradient texture for a given grid (defined by 'range') and store it into 4x4 texture

	glm::vec4 func_range = this->GetGrid()->GetRangeAsOriginStep();

	glm::vec4 clear_color;
	glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
	glClearColor( 0, 0, 0, 0 );

	m_scene->GetPassPtr( "pass_func_values" )->Activate();

	glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

	if( this->IsEnabled() )
	{
		if ( (string) this->GetName() ==  "Spline" )
		{
			func_range.x +=  func_range.y;
			func_range.z +=  func_range.w;

			glViewport( 0+2, 0+2, this->GetGrid()->GetControlPointsCount()-2, this->GetGrid()->GetControlPointsCount()-2 );
			m_scene->SetUniform("mat_get_2Dfunc_values", "grid_res", (float) this->GetGrid()->GetControlPointsCount() - 2.0 );
		}
		else
		{
			glClear(GL_COLOR_BUFFER_BIT);
			m_scene->SetUniform("mat_get_2Dfunc_values", "grid_res", (float) this->GetGrid()->GetControlPointsCount() );
		}

#ifdef GRADIENT_METHOD
		SetUniform("mat_get_2Dfunc_values", "range", func_range );	
		RenderPass("mat_get_2Dfunc_values");
#else
		glBindTexture( GL_TEXTURE_2D, TextureCache::Instance()->Get( "MTEX_2Dfunc_values" ) );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->GetControlPointsCount(), this->GetControlPointsCount(), 0, GL_RGBA, GL_FLOAT, g_precomputed_diffs);
		glBindTexture( GL_TEXTURE_2D, 0 );
#endif

	}

	m_scene->GetPassPtr( "pass_func_values" )->Deactivate();

	if ( (string) this->GetName() ==  "Spline" )
		//-- 0, 0, 19, 19
		glViewport( 0, 0, this->GetControlPointsCount(), this->GetControlPointsCount() );

	//-- Simplified deformation model 
	/*
	sigma = 1.0;
	AddTexture("mat_aliasblur_horiz","MTEX_2Dfunc_values",RENDER_TEXTURE);
	SetUniform("mat_aliasblur_horiz", "texsize", glm::ivec2(m_shadow_technique->GetResolution()));
	SetUniform("mat_aliasblur_horiz", "kernel_size", 3.0);
	SetUniform("mat_aliasblur_horiz", "two_sigma_sq", TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_horiz", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));
	AddTexture("mat_aliasblur_vert","MTEX_2Dfunc_values_ping",RENDER_TEXTURE);
	SetUniform("mat_aliasblur_vert", "texsize", glm::ivec2(m_shadow_technique->GetResolution()));
	SetUniform("mat_aliasblur_vert", "kernel_size", 3.0);
	SetUniform("mat_aliasblur_vert", "two_sigma_sq", TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_vert", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values_ping"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	RenderPass("mat_aliasblur_horiz");
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"], 0);
	glClear(GL_COLOR_BUFFER_BIT);
	RenderPass("mat_aliasblur_vert");

	RemoveTexture("mat_aliasblur_horiz","MTEX_2Dfunc_values");
	RemoveTexture("mat_aliasblur_vert","MTEX_2Dfunc_values_ping");
	*/

	///////////////////////////////////////////////////////////////////////////////
	//-- 6. compute 2D polynomial coefficents and store them into textures (gradient for x and y axes)
	//-- odtud zacina cast, ktera je pro kazdou warpovaci metodu jina. Proto se pouziva abstraktni trida shadow_technique

	this->SetTexId(TextureCache::Instance()->Get( "MTEX_2Dfunc_values" ));

	//-- vrati pocet ridicich bodu mrizky, 
	int res = (int)this->GetControlPointsCount();
	m_pFuncValues = new float[res*res*2];
	
	memset(m_pFuncValues, 0, res*res*2*sizeof(float));

	glBindTexture(GL_TEXTURE_2D, m_iTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, m_pFuncValues);
	glBindTexture(GL_TEXTURE_2D, 0 ); 

	//FIXME:
	//-- kod vyse se vola jen pro to, ze metoda GenerateGrid vola stejnojmennou metodu m_screen_grid,
	//-- kde se vola ciste virtualni metodu IWarpedShadowTechnique, kde v teto konretni tride se m_pFuncValues
	//-- pouzije.
	this->GenerateGrid();

	delete [] m_pFuncValues;
	m_pFuncValues = NULL;

	///////////////////////////////////////////////////////////////////////////////


	glEnable(GL_DEPTH_TEST);

	glViewport( 0, 0, sh_res, sh_res );
	//glColorMask(0, 0, 0, 0);      //disable colorbuffer write
	glCullFace(GL_FRONT);
	glEnable(GL_CLIP_PLANE0);

	if(m_scene->IsWireframe())
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	m_scene->GetPassPtr( "pass_warped_depth" )->Activate();

	for(int i=0; i<2; i++)
	{
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  TextureCache::Instance()->Get("MTEX_warped_depth_color"), 0, i);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  TextureCache::Instance()->Get("tex_shadow"), 0, i);	    

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-- Shared uniforms
		m_scene->SetUniform("mat_depth_with_warping", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
		m_scene->SetUniform("mat_depth_with_warping", "range", this->GetGridRange());
		m_scene->SetUniform("mat_depth_with_warping", "grid_res", (float) this->GetControlPointsCount() );

		//-- Polynomial uniforms
		m_scene->SetUniform("mat_depth_with_warping", "coeffsX", coeffsX );
		m_scene->SetUniform("mat_depth_with_warping", "coeffsY", coeffsY );

		//-- Bilinear uniforms
		//SetUniform("mat_depth_with_warping", "funcTex", 1 );

		float z_direction = 1.0;
		if(i == 1)
			z_direction = -1.0;  
		lightViewMatrix[i] = glm::lookAt(m_pLight->GetPos(), m_pLight->GetPos() + glm::vec3( m_scene->GetCameraPtr()->GetFarPlane()*z_direction, 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );

		/*
		if(m_dpshadow_tess)
		{
			m_scene->SetUniform("_mat_shadow_warp_tess", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
			m_scene->SetUniform("_mat_shadow_warp_tess", "coeffsX", coeffsX );
			m_scene->SetUniform("_mat_shadow_warp_tess", "coeffsY", coeffsY );

			m_scene->DrawSceneDepth("_mat_shadow_warp_tess", lightViewMatrix[i]);
		}
		else
		*/
		{
			m_scene->DrawGeometry("mat_depth_with_warping", lightViewMatrix[i]);
		}
	}
	m_scene->GetPassPtr( "pass_warped_depth" )->Deactivate();

	if( !m_scene->IsWireframe() )
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	//Finish, restore values
	glDisable( GL_CLIP_PLANE0 );
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);

	///////////////////////////////////////////////////////////////////////////////

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0,0,sh_res,sh_res);
	//RenderPass("mat_quad");

	//end draw_aliaserror

	/*
	//Finish, restore values
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);
	glViewport(0, 0, m_resx, m_resy);         //reset viewport

	///3. Calculate shadow texture matrix
	glm::mat4 shadowMatrix = biasMatrix * lightProjMatrix * lightViewMatrix * glm::inverse(m_viewMatrix);

	//restore projection matrix and set shadow matrices
	glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, (l->GetOrd() + 1) * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shadowMatrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	*/

	//set light matrices and near/far planes to all materials
	for(m_im = m_materials.begin(); m_im != m_materials.end(); ++m_im)
	{
		if(m_im->second->GetSceneID() == m_sceneID && !m_im->second->IsScreenSpace())
		{
			m_im->second->SetUniform("lightModelView[0]", lightViewMatrix[0]);
			m_im->second->SetUniform("lightModelView[1]", lightViewMatrix[1]);
			m_im->second->SetUniform("near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
			m_im->second->SetUniform("coeffsX", coeffsX );
			m_im->second->SetUniform("coeffsY", coeffsY );
			m_im->second->SetUniform("range", this->GetGridRange());
			m_im->second->SetUniform("grid_res", (float) this->GetControlPointsCount());
		}
	}

	//-- set debug shaders
	m_scene->SetUniform("mat_aliasError", "lightModelView[0]", lightViewMatrix[0]);
	m_scene->SetUniform("mat_aliasError", "lightModelView[1]", lightViewMatrix[1]);
	m_scene->SetUniform("mat_aliasError", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	m_scene->SetUniform("mat_aliasError", "grid_res", (float) this->GetControlPointsCount());
	m_scene->SetUniform("mat_aliasError", "range", this->GetGridRange());
	//-- set debug shaders
	//SetUniform("mat_perspective_error", "lightModelView", glm::lookAt( l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) ));
	m_scene->SetUniform("mat_perspective_error", "lightModelView[0]", lightViewMatrix[0]);
	m_scene->SetUniform("mat_perspective_error", "lightModelView[1]", lightViewMatrix[1]);
	m_scene->SetUniform("mat_perspective_error", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	m_scene->SetUniform("mat_perspective_error", "camera_space_light_position", cam_view_matrix * glm::vec4( m_pLight->GetPos(), 1.0 ));
	m_scene->SetUniform("mat_perspective_error", "matrix_ortho", glm::ortho(-512.f, 512.f, -512.f, 512.f, 0.1f, 10000.0f) );

}

//----------------------------------------------------------------------------

void SplineWarpedShadow::PostRender()
{
}

//----------------------------------------------------------------------------

//-- _P je v intervalu [0..grid_res-1]
glm::vec2 SplineWarpedShadow::ComputeDiff( glm::vec2 _P )
{
	glm::vec2 diff( 0.0 );

	if( m_iTexID == 0 || m_pFuncValues == NULL || !m_bEnabled ) return diff;

	//-- vypocet souradnice bunky, ve ktere se bod _P nachazi
	glm::vec2 grid_coords = glm::floor( _P );
	
	glm::mat4 P;
	glm::vec4 Tx, Ty;
	glm::vec4 Q;

	Splines4x4 splines;

	//-- prevod do intervalu [0..1], tj. pozice v bunce
	float x = glm::fract(_P.x);
	float y = glm::fract(_P.y);

	splines = _GetSplinesValues( grid_coords );

	Tx = glm::vec4( 1.0, x, glm::pow(x, 2.0f), glm::pow(x,3.0f) );
	Ty = glm::vec4( 1.0, y, glm::pow(y, 2.0f), glm::pow(y, 3.0f) );

	//-- diff X
	
	//-- definice po sloupcich
	P = glm::mat4(
		splines.p0[0].x,	splines.p1[0].x,	splines.p2[0].x,	splines.p3[0].x,
		splines.p0[1].x,	splines.p1[1].x,	splines.p2[1].x,	splines.p3[1].x,
		splines.p0[2].x,	splines.p1[2].x,	splines.p2[2].x,	splines.p3[2].x,
		splines.p0[3].x,	splines.p1[3].x,	splines.p2[3].x,	splines.p3[3].x
		);

	Q = P * Mcr * Tx;
	diff.x = glm::dot(Q * Mcr, Ty);

	//-- diff Y

	//-- definice po sloupcich
	P = glm::mat4(
		splines.p0[0].y,	splines.p1[0].y,	splines.p2[0].y,	splines.p3[0].y,
		splines.p0[1].y,	splines.p1[1].y,	splines.p2[1].y,	splines.p3[1].y,
		splines.p0[2].y,	splines.p1[2].y,	splines.p2[2].y,	splines.p3[2].y,
		splines.p0[3].y,	splines.p1[3].y,	splines.p2[3].y,	splines.p3[3].y
		);

	Q = P * Mcr * Tx;
	glm::vec4 v = Mcr * Ty;
	diff.y = glm::dot(Q * Mcr, Ty);

	return diff * POLY_BIAS;
}

#define I(i,j) (2*( (j)*(res) + (i) ))
Splines4x4 SplineWarpedShadow::_GetSplinesValues( glm::vec2 _c )
{

	Splines4x4 ret;

	if( m_pFuncValues == NULL ) 
		return ret;

	//-- v tomto pridade se bere skutecne rozliseni, tj. rozliseni textury
	//CHYBA
	int res = (int)this->GetControlPointsCount();
	int x = (int)_c.x, y = (int)_c.y;
	assert( _c.x<res && _c.y<res );

	//-- jedna bunka
	// P3  ----
	//     |  |
	// P2  ----
	//     |XX|
	// P1  ----
	//     |  |
	// P0  ----

	memcpy( glm::value_ptr(ret.p0), m_pFuncValues + I(x,y + 0), sizeof(glm::mat4x2) );
	memcpy( glm::value_ptr(ret.p1), m_pFuncValues + I(x,y + 1), sizeof(glm::mat4x2) );
	memcpy( glm::value_ptr(ret.p2), m_pFuncValues + I(x,y + 2), sizeof(glm::mat4x2) );
	memcpy( glm::value_ptr(ret.p3), m_pFuncValues + I(x,y + 3), sizeof(glm::mat4x2) );

	return ret;
}

float SplineWarpedShadow::GetControlPointsCount()
{
	//-- potrebujeme o jedno vetsi rozliseni na kazde strane kvuli navic bodum pro spline
	return (IWarpedShadowTechnique::GetControlPointsCount() + 2.0);
}
