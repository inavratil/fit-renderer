#include "SplineWarpedShadow.h"

#include "shadow.h"
#include "shaderGen/SFSplineWarpedShadow.h"
#include "resources/TextureCache.h"
#include "scene.h"

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
