#include "glux_engine/scene.h"

#include "glux_engine/PolynomialWarpedShadow.h"
#include "glux_engine/BilinearWarpedShadow.h"
#include "glux_engine/SplineWarpedShadow.h"

#include "glux_engine/BlitPass.h"
#include "glux_engine/SimplePass.h"

#include "precomputed_diffs.h"

///////////////////////////////////////////////////////////////////////////////
//-- Defines

//#define DEBUG_DRAW 
//#define GRADIENT_METHOD
//#define ITERATION_ENABLED

///////////////////////////////////////////////////////////////////////////////
//-- Global variables

glm::vec4 g_precomputed_diffs[19*19];

///////////////////////////////////////////////////////////////////////////////


const float COVERAGE[100] = { 0.2525314589560607, 0.2576326279264187, 0.2628360322702068, 0.2681431353083358, 0.2735553772432651, 0.2790741715891978, 0.2847009014014726, 0.2904369152983624, 0.2962835232704883, 0.3022419922728888, 0.308313541593817, 0.31449933799764, 0.3208004906361242, 0.3272180457262481, 0.3337529809911264, 0.3404061998622743, 0.3471785254421841, 0.3540706942267695,
	0.3610833495887235, 0.3682170350233647, 0.3754721871601566, 0.3828491285443266, 0.3903480601940126, 0.3979690539407849, 0.4057120445616824, 0.4135768217143915, 0.42156302168711, 0.4296701189785898, 0.437897417723999, 0.4462440429868523, 0.4547089319362121, 0.463290824935085, 0.4719882565643009, 0.4807995466116144, 0.4897227910574863, 0.4987558530912262, 0.5078963541952966, 0.5171416653374513,
	0.5264888983132224, 0.5359348972850012, 0.5454762305660166, 0.5551091827004417, 0.5648297468943986, 0.5746336178533678, 0.584516185086649, 0.5944725267394274, 0.6044974040160985, 0.6145852562605928, 0.6247301967608438, 0.6349260093447057, 0.64516614583707, 0.6554437244475375, 0.6657515291573695, 0.6760820101752764, 0.6864272855286133, 0.6967791438574159, 0.7071290484729112, 0.7174681427427703,
	0.7277872568587884, 0.7380769160403831, 0.748327350219802, 0.7585285052521402, 0.7686700556833742, 0.7787414191045477, 0.7887317721106483, 0.7986300678752298, 0.8084250553400247, 0.8181053000119952, 0.8276592063450691, 0.8370750416769379, 0.846340961676199, 0.8554450372439244, 0.8643752828030637, 0.87311968589294, 0.8816662379765975, 0.890002966355447, 0.8981179670715378, 0.9059994386685738,
	0.9136357166693593, 0.9210153086167311, 0.9281269295161807, 0.9349595375079411, 0.9415023695886816, 0.9477449771968978, 0.9536772614690937, 0.9592895079717747, 0.9645724207097471, 0.9695171552121149, 0.9741153504974227, 0.9783591597219075, 0.9822412793198499, 0.9857549764505119, 0.9888941145749449, 0.991653176994985, 0.9940272881980572, 0.9960122328654299, 0.9976044724143962, 0.9988011589619574,
	0.9996001466136796, 1 };

void ModifyGrid(glm::vec4 *precomputed_diffs);

/**
****************************************************************************************************
@brief Creates shadow map and other structures for selected light for multiresolution rendering
@param ii iterator to lights list
@return success/fail of shadow creation
****************************************************************************************************/
bool TScene::WarpedShadows_InitializeTechnique(vector<TLight*>::iterator ii)
{
	int sh_res = (*ii)->ShadowSize();

	//-- Polynomial shadow technique
	//this->SetShadowTechnique( new PolynomialWarpedShadow() );

	//-- Bilinear shadow technique
	//this->SetShadowTechnique( new BilinearWarpedShadow() );
	//-- Spline shadow technique
	this->SetShadowTechnique( new SplineWarpedShadow() );
	m_shadow_technique->SetResolution( 17.0 );

	//create data textures
	try{

		m_texture_cache->Create2DManual( "tex_camAndLightCoords", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "tex_stencil_color", 128, 128, GL_RGBA16F,	GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "tex_output", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "MTEX_ping", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "MTEX_pong", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		//FIXME: precision??? nestaci 16F ?
		m_texture_cache->Create2DManual( "MTEX_2Dfunc_values", m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution(), GL_RGBA32F, GL_FLOAT, GL_NEAREST, false );
		m_texture_cache->Create2DManual( "MTEX_2Dfunc_values_ping", m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution(), GL_RGBA32F, GL_FLOAT, GL_NEAREST, false );
		
		TexturePtr tex_shadow = m_texture_cache->Create2DArrayManual("tex_shadow",
			sh_res, sh_res,			//-- width and height
			2,						//-- number of layers
			GL_DEPTH_COMPONENT,		//-- internal format
			GL_FLOAT,				//-- type of the date
			GL_NEAREST,				//-- filtering
			false					//-- mipmap generation
			);
		tex_shadow->SetType( SHADOW_OMNI );
		tex_shadow->SetIntensity( (*ii)->ShadowIntensity() );
		m_texture_cache->Create2DArrayManual("MTEX_warped_depth_color",
			sh_res, sh_res,	//-- width and height
			2,				//-- number of layers
			GL_RGBA16F,		//-- internal format
			GL_FLOAT,		//-- type of the date
			GL_NEAREST,		//-- filtering
			false			//-- mipmap generation
			);
		m_texture_cache->Create2DManual("aliaserr_mipmap",
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

#ifdef ITERATION_ENABLED
			CustomShader("mat_camAndLightCoords_afterDP","data/shaders/warping/camAndLightCoords_afterDP.vert", "data/shaders/warping/camAndLightCoords_afterDP.frag", m_shadow_technique->GetDefines(), "");
#else
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_camAndLightCoords_afterDP","data/shaders/warping/camAndLightCoords_afterDP.vert", "data/shaders/warping/camAndLightCoords_afterDP.frag" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ), "funcTex" );
			AddMaterial( mat );
#endif
			//-- pass
			SimplePassPtr pass_coords = new SimplePass( sh_res/8, sh_res/8 );
			pass_coords->AttachOutputTexture( 0, m_texture_cache->GetPtr( "tex_camAndLightCoords" ) );
			pass_coords->AttachOutputTexture( 1, m_texture_cache->GetPtr( "tex_stencil_color" ) );
			pass_coords->SetShader( mat );
			AppendPass("pass_coords", pass_coords );
		}

		//-----------------------------------------------------------------------------
		//-- output
		{				
			//-- shader
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_compute_aliasError", "data/shaders/warping/computeAliasError.vert"/*quad.vert*/, "data/shaders/warping/computeAliasError.frag" );
			mat->AddTexture( m_texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "tex_error" );
			mat->AddTexture( m_texture_cache->GetPtr( "tex_camAndLightCoords" ), "tex_coords" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_compute_aliasError = new SimplePass( sh_res/8, sh_res/8 );
			pass_compute_aliasError->AttachOutputTexture( 0, m_texture_cache->GetPtr( "tex_output" ) );
			pass_compute_aliasError->DisableDepthBuffer();
			pass_compute_aliasError->SetShader( mat );
			AppendPass("pass_compute_aliasError", pass_compute_aliasError );
		}
		//-----------------------------------------------------------------------------

		//FIXME: Tohle by melo prijit do Init metody dane shadow techniky
		m_shadow_technique->GetShaderFeature()->AddTexture( "MTEX_2Dfunc_values", m_texture_cache->Get("MTEX_2Dfunc_values"), 1.0, ShaderFeature::FS );

		//FIXME: to shadowID by se mohlo/melo nastavovat jinde
		(*ii)->SetShadowTexID( m_texture_cache->Get( "tex_shadow" ) );

		//-----------------------------------------------------------------------------
		//blur
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasblur_horiz","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ","#define HORIZONTAL\n" );
			mat->AddTexture( m_texture_cache->GetPtr( "tex_output" ), "bloom_texture" );
			AddMaterial( mat );

			//-- pass
			SimplePassPtr pass_horiz_blur = new SimplePass( sh_res/8, sh_res/8 );
			pass_horiz_blur->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_ping" ) );
			pass_horiz_blur->DisableDepthBuffer();
			AppendPass("pass_horiz_blur", pass_horiz_blur );
		}
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasblur_vert","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ", "#define VERTICAL\n" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_ping" ), "bloom_texture" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_vert_blur = new SimplePass( sh_res/8, sh_res/8 );
			pass_vert_blur->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_pong" ) );
			pass_vert_blur->DisableDepthBuffer();
			AppendPass("pass_vert_blur", pass_vert_blur );
		}
		//-----------------------------------------------------------------------------
		//alias gradient
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasgradient","data/shaders/quad.vert","data/shaders/warping/aliasgradient.frag" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_pong" ), "tex_blurred_error" );
			AddMaterial( mat );	
			//-- pass
			SimplePassPtr pass_gradient = new SimplePass( sh_res/8, sh_res/8 );
			pass_gradient->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_ping" ) );
			pass_gradient->DisableDepthBuffer();
			AppendPass("pass_gradient", pass_gradient );
		}
		//-----------------------------------------------------------------------------
		//2D func values
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_get_2Dfunc_values","data/shaders/quad.vert","data/shaders/warping/get2DfuncValues.frag" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_ping" ), "gradient_map" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_func_values = new SimplePass( m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution() );
			pass_func_values->AttachOutputTexture( 0, m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ) );
			pass_func_values->DisableDepthBuffer();
			AppendPass("pass_func_values", pass_func_values );
		}
		//-----------------------------------------------------------------------------
		//draw depth with warping
		{
			ScreenSpaceMaterial* mat = 
				new ScreenSpaceMaterial( 
				"mat_depth_with_warping",
				"data/shaders/warping/drawDepthWithWarping.vert", "data/shaders/warping/drawDepthWithWarping.frag", 
				m_shadow_technique->GetDefines(), "" 
				);
			mat->AddTexture( m_texture_cache->GetPtr("MTEX_2Dfunc_values"), "funcTex" );
			AddMaterial( mat );
			//-- pass
			SimplePassPtr pass_warped_depth = new SimplePass( sh_res, sh_res );
			pass_warped_depth->AttachOutputTexture( 0, m_texture_cache->GetPtr("MTEX_warped_depth_color") );
			pass_warped_depth->AttachOutputTexture( 0, m_texture_cache->GetPtr( "tex_shadow" ), true );
			AppendPass("pass_warped_depth", pass_warped_depth );	
		}
		//-----------------------------------------------------------------------------
		//-- blit pass
		//BlitPass *bp = new BlitPass( 128, 128 );
		//bp->AttachReadTexture( m_texture_cache->GetPtr( "tex_output" ) );
		//bp->AttachDrawTexture( m_texture_cache->GetPtr( "aliaserr_mipmap" ) );
		//AppendPass("pass_blit_0", bp );
		//-- copy squared error
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_copy_squared_error", "data/shaders/quad.vert", "data/shaders/warping/copy_squared_error.frag" );
			mat->AddTexture( m_texture_cache->GetPtr("tex_output"), "tex_alias_error" );
			AddMaterial( mat );

			//-- pass
			SimplePass *mp = new SimplePass( 128, 128 );
			mp->AttachOutputTexture(0, m_texture_cache->GetPtr("aliaserr_mipmap") ); 
			mp->DisableDepthBuffer();
			mp->SetShader( mat );
			AppendPass("pass_copy_squared_error", mp);
		}
		//-----------------------------------------------------------------------------
		//-- mipmap pass
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasMipmap", "data/shaders/quad.vert", "data/shaders/shadow_alias_mipmap.frag" );
			mat->AddTexture( m_texture_cache->GetPtr("aliaserr_mipmap"), "mat_aliasError" );
			AddMaterial( mat );

			//-- pass
			SimplePass *mp = new SimplePass( 128, 128 );
			mp->AttachOutputTexture(0, m_texture_cache->GetPtr("aliaserr_mipmap") ); 
			mp->DisableDepthBuffer();
			AppendPass("pass_alias_mipmap", mp);
		}
		//-----------------------------------------------------------------------------
		{
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "show_depth_omni", "data/shaders/showDepth.vert", "data/shaders/showDepth_omni.frag" ); 
			mat->AddTexture( m_texture_cache->GetPtr("MTEX_warped_depth_color"), "show_depth_dpShadowA" );
			AddMaterial( mat );
		}
		//-----------------------------------------------------------------------------
		{
			//-- shader showing shadow map alias error
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_aliasError", "data/shaders/shadow_alias_error.vert", "data/shaders/shadow_alias_error.frag" );
			mat->AddTexture( m_texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "mat_aliasErrorBaseA" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ), "MTEX_2Dfunc_values" );
			AddMaterial( mat );
		}
		{
			//-- shader showing perspective alias error
			ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_perspective_error", "data/shaders/perspective_alias_error.vert", "data/shaders/perspective_alias_error.frag" );
			mat->AddTexture( m_texture_cache->CreateFromImage( "data/tex/error_color.tga" ), "tex_error_color" );
			mat->AddTexture( m_texture_cache->GetPtr( "MTEX_2Dfunc_values" ), "tex_2Dfunc_values" );
			AddMaterial( mat );
		}
	}
	catch(int)
	{
		return false;
	}
	return true;
}

/**
****************************************************************************************************
@brief Render omnidirectional shadow map using multiresolution techniques
@param l current light
****************************************************************************************************/
void TScene::WarpedShadows_RenderShadowMap(TLight *l)
{
	int sh_res = l->ShadowSize();
	GLenum mrt[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	//set light projection and light view matrix
	glm::mat4 lightProjMatrix = glm::perspective(90.0f, 1.0f, 1.0f, 1000.0f);
	glm::mat4 lightViewMatrix[2];
	lightViewMatrix[1] = glm::lookAt(l->GetPos(), l->GetPos() + glm::vec3(-m_cam->GetFarPlane(), 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );
	//glm::mat4 lightViewMatrix = glm::lookAt(l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );

	//glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjMatrix));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 coeffsX = glm::mat4( 0.0 );
	glm::mat4 coeffsY = glm::mat4( 0.0 );
	glm::vec4 mask_range = glm::vec4( 128.0, 0.0, 128.0, 0.0 );

#ifdef ITERATION_ENABLED
	for( int rep=0; rep<5; ++rep )
#endif

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

	m_passes["pass_coords"]->Activate();
	MaterialPtr mat_coords = m_passes["pass_coords"]->GetShader();
	mat_coords->SetUniform("cam_mv", m_viewMatrix );
	mat_coords->SetUniform("cam_proj", m_projMatrix );
	mat_coords->SetUniform("near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	mat_coords->SetUniform("grid_res", (float) m_shadow_technique->GetResolution() );
	mat_coords->SetUniform("matrix_ortho", glm::ortho(-512.f, 512.f, -512.f, 512.f, 0.1f, 10000.0f) );
	mat_coords->SetUniform("camera_space_light_position", m_viewMatrix * glm::vec4( l->GetPos(), 1.0 )); 

	DrawGeometry(mat_coords->GetName().c_str(), lightViewMatrix[1]);

	m_passes["pass_coords"]->Deactivate();

	//if(!m_wireframe)
	//	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glCullFace(GL_BACK);
	glDisable( GL_CULL_FACE);

	

	//-----------------------------------------------------------------------------

	//FIXME: tenhle kod by mel prijit do PreRender metody, ale zatim neni vyresen pristup globalni pristup k texturam
	if ( (string) m_shadow_technique->GetName() ==  "Polynomial" )
	{
		float mask_values[128*128];
		glBindTexture(GL_TEXTURE_2D, m_texture_cache->Get( "tex_stencil_color" ) );
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

	///////////////////////////////////////////////////////////////////////////////
	//-- 2. Compute alias error based on the coordinates data computed in step 1
	//--	Input: tex_camAndLightCoords (error texture)

	//FIXME: opravdu mohu pouzit tex_coords v plnem rozliseni, pri mensim rozliseni se bude brat jenom kazda n-ta hodnota
	//		textureOffset se pousouva v rozliseni textury, takze kdyz je rozliseni 1024, posune se o pixel v tomto rozliseni
	//		pri rendrovani do 128x128 je mi toto chovani ale na prd ?????
	m_passes["pass_compute_aliasError"]->Activate();
	m_passes["pass_compute_aliasError"]->Render();
	m_passes["pass_compute_aliasError"]->Deactivate();	

#ifndef GRADIENT_METHOD
	//calculate custom mipmaps 
	{
		//-----------------------------------------------------------------------------

		//m_passes["pass_blit_0"]->Process();
		m_passes["pass_copy_squared_error"]->Activate();
		m_passes["pass_copy_squared_error"]->Render(); //RenderPass("mat_copy_squared_error");		
		m_passes["pass_copy_squared_error"]->Deactivate();

		//-----------------------------------------------------------------------------

		glm::vec4 clear_color;
		glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
		glClearColor( 1, 1, 1, 1 );

		glColorMask( 0, 0, 0, 0 );	//-- nutno vypnout, aby se neclearovalo FBO
		m_passes["pass_alias_mipmap"]->Activate();
		glColorMask( 1, 1, 1, 1 );	//-- znovu zapnout

		for(int i=1, j = 128.0/2; j>=1; i++, j/=2)
		{
			glViewport(0, 0, j, j);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_cache->Get("aliaserr_mipmap" ), i);
			glClear(GL_COLOR_BUFFER_BIT );

			SetUniform("mat_aliasMipmap", "offset", 0.5f/((float)j*2.0f));
			SetUniform("mat_aliasMipmap", "mip_level", i-1);
			RenderPass("mat_aliasMipmap");				
		}
	
		m_passes["pass_alias_mipmap"]->Deactivate();


		glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

		//-----------------------------------------------------------------------------

		memset(g_precomputed_diffs, 0, 19*19*sizeof(glm::vec4));
		ModifyGrid(g_precomputed_diffs);

		glViewport( 0, 0, 128.0, 128.0 ); //restore viewport
	}
#endif


	///////////////////////////////////////////////////////////////////////////////
	//-- 3. Blur the alias error
	//--	input: MTEX_ouput (horiz), MTEX_ping (vert)

	glDisable(GL_DEPTH_TEST);

	float sigma = 2.7;

	m_passes["pass_horiz_blur"]->Activate();

	//GetMaterial( "mat_aliasblur_horiz" )->AddTexture( m_texture_cache->GetPtr( "tex_output" ), "bloom_texture" );
	SetUniform("mat_aliasblur_horiz", "texsize", glm::ivec2(sh_res/8, sh_res/8));
	SetUniform("mat_aliasblur_horiz", "kernel_size", 9.0);
	SetUniform("mat_aliasblur_horiz", "two_sigma_sq", TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_horiz", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	RenderPass("mat_aliasblur_horiz");

	m_passes["pass_horiz_blur"]->Deactivate();

	//-----------------------------------------------------------------------------

	m_passes["pass_vert_blur"]->Activate();

	//GetMaterial( "mat_aliasblur_vert" )->AddTexture( m_texture_cache->GetPtr( "MTEX_ping" ), "bloom_texture" );
	SetUniform("mat_aliasblur_vert", "texsize", glm::ivec2(sh_res/8, sh_res/8));
	SetUniform("mat_aliasblur_vert", "kernel_size", 9.0);
	SetUniform("mat_aliasblur_vert", "two_sigma_sq",TWOSIGMA2(sigma));
	SetUniform("mat_aliasblur_vert", "frac_sqrt_two_sigma_sq", FRAC_TWOPISIGMA2(sigma));

	RenderPass("mat_aliasblur_vert");

	m_passes["pass_vert_blur"]->Deactivate();

	//GetMaterial( "mat_aliasblur_horiz" )->DeleteTexture( "bloom_texture" );
	//GetMaterial( "mat_aliasblur_vert" )->DeleteTexture( "bloom_texture" );

	///////////////////////////////////////////////////////////////////////////////
	//-- 4. Compute gradient and store the result per-pixel into 128x128 texture

	glm::vec2 limit = m_shadow_technique->GetGrid()->GetOffset() / 128.0f;
	limit /= POLY_BIAS;

	m_passes["pass_gradient"]->Activate();

	//FIXME: limit pocitat nejak inteligentne bez bulharske konstanty
	//SetUniform("mat_aliasgradient", "limit", glm::vec2(100.0f));
	SetUniform("mat_aliasgradient", "limit", limit);
	RenderPass("mat_aliasgradient");

	m_passes["pass_gradient"]->Deactivate();

	///////////////////////////////////////////////////////////////////////////////
	//-- 5. get a function value from gradient texture for a given grid (defined by 'range') and store it into 4x4 texture

	glm::vec4 func_range = m_shadow_technique->GetGrid()->GetRangeAsOriginStep();

	glm::vec4 clear_color;
	glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
	glClearColor( 0, 0, 0, 0 );

	m_passes["pass_func_values"]->Activate();

	glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

	if( m_shadow_technique->IsEnabled() )
	{
		if ( (string) m_shadow_technique->GetName() ==  "Spline" )
		{
			func_range.x +=  func_range.y;
			func_range.z +=  func_range.w;

			glViewport( 0+2, 0+2, m_shadow_technique->GetGrid()->GetResolution()-2, m_shadow_technique->GetGrid()->GetResolution()-2 );
			SetUniform("mat_get_2Dfunc_values", "grid_res", (float) m_shadow_technique->GetGrid()->GetResolution() - 2.0 );
		}
		else
		{
			glClear(GL_COLOR_BUFFER_BIT);
			SetUniform("mat_get_2Dfunc_values", "grid_res", (float) m_shadow_technique->GetGrid()->GetResolution() );
		}

#ifdef GRADIENT_METHOD
		SetUniform("mat_get_2Dfunc_values", "range", func_range );	
		RenderPass("mat_get_2Dfunc_values");
#else
		glBindTexture( GL_TEXTURE_2D, m_texture_cache->Get( "MTEX_2Dfunc_values" ) );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution(), 0, GL_RGBA, GL_FLOAT, g_precomputed_diffs);
		glBindTexture( GL_TEXTURE_2D, 0 );
#endif

	}

	m_passes["pass_func_values"]->Deactivate();

	if ( (string) m_shadow_technique->GetName() ==  "Spline" )
		glViewport( 0, 0, m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution() );

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

	m_shadow_technique->SetTexId(m_texture_cache->Get( "MTEX_2Dfunc_values" ));
	m_shadow_technique->PreRender();

	///////////////////////////////////////////////////////////////////////////////

#ifdef ITERATION_ENABLED
	RenderDebug();

	cout << rep << endl;
	SDL_Delay( 100 );
	//std::string dummy;
	//std::getline(std::cin, dummy);

	glViewport(0,0,1024,1024);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_texture"));                
	RenderPass("mat_quad");
	glBindTexture(GL_TEXTURE_2D, 0);

	m_shadow_technique->DrawGrid();

	SDL_GL_SwapBuffers();
#endif
	glEnable(GL_DEPTH_TEST);

#ifdef ITERATION_ENABLED
	cout << endl;
#endif
	glViewport( 0, 0, sh_res, sh_res );
	//glColorMask(0, 0, 0, 0);      //disable colorbuffer write
	glCullFace(GL_FRONT);
	glEnable(GL_CLIP_PLANE0);

	if(m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	m_passes["pass_warped_depth"]->Activate();

	for(int i=0; i<2; i++)
	{
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  m_texture_cache->Get("MTEX_warped_depth_color"), 0, i);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_texture_cache->Get("tex_shadow"), 0, i);	    

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-- Shared uniforms
		SetUniform("mat_depth_with_warping", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
		SetUniform("mat_depth_with_warping", "range", m_shadow_technique->GetGridRange());
		SetUniform("mat_depth_with_warping", "grid_res", (float) m_shadow_technique->GetResolution() );

		//-- Polynomial uniforms
		SetUniform("mat_depth_with_warping", "coeffsX", coeffsX );
		SetUniform("mat_depth_with_warping", "coeffsY", coeffsY );

		//-- Bilinear uniforms
		//SetUniform("mat_depth_with_warping", "funcTex", 1 );

		float z_direction = 1.0;
		if(i == 1)
			z_direction = -1.0;  
		lightViewMatrix[i] = glm::lookAt(l->GetPos(), l->GetPos() + glm::vec3( m_cam->GetFarPlane()*z_direction, 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );


		if(m_dpshadow_tess)
		{
			SetUniform("_mat_shadow_warp_tess", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
			SetUniform("_mat_shadow_warp_tess", "coeffsX", coeffsX );
			SetUniform("_mat_shadow_warp_tess", "coeffsY", coeffsY );

			DrawSceneDepth("_mat_shadow_warp_tess", lightViewMatrix[i]);
		}
		else
		{
			DrawGeometry("mat_depth_with_warping", lightViewMatrix[i]);
		}
	}
	m_passes["pass_warped_depth"]->Deactivate();

	if(!m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	//Finish, restore values
	glDisable( GL_CLIP_PLANE0 );
	glCullFace(GL_BACK);
	glColorMask(1, 1, 1, 1);


#ifdef ITERATION_ENABLED
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbos["ipsm"]);		
	glViewport( 0, 0, m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution() );
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"], 0);
	glClear(GL_COLOR_BUFFER_BIT );
	glBindFramebuffer( GL_FRAMEBUFFER, 0);
#endif

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
			m_im->second->SetUniform("range", m_shadow_technique->GetGridRange());
			m_im->second->SetUniform("grid_res", (float) m_shadow_technique->GetResolution());
		}
	}

	//-- set debug shaders
	SetUniform("mat_aliasError", "lightModelView[0]", lightViewMatrix[0]);
	SetUniform("mat_aliasError", "lightModelView[1]", lightViewMatrix[1]);
	SetUniform("mat_aliasError", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	SetUniform("mat_aliasError", "grid_res", (float) m_shadow_technique->GetResolution());
	SetUniform("mat_aliasError", "range", m_shadow_technique->GetGridRange());
	//-- set debug shaders
	//SetUniform("mat_perspective_error", "lightModelView", glm::lookAt( l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) ));
	SetUniform("mat_perspective_error", "lightModelView[0]", lightViewMatrix[0]);
	SetUniform("mat_perspective_error", "lightModelView[1]", lightViewMatrix[1]);
	SetUniform("mat_perspective_error", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	SetUniform("mat_perspective_error", "camera_space_light_position", m_viewMatrix * glm::vec4( l->GetPos(), 1.0 ));
	SetUniform("mat_perspective_error", "matrix_ortho", glm::ortho(-512.f, 512.f, -512.f, 512.f, 0.1f, 10000.0f) );
}
