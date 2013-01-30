#include "glux_engine/scene.h"

#include "glux_engine/PolynomialWarpedShadow.h"
#include "glux_engine/BilinearWarpedShadow.h"

///////////////////////////////////////////////////////////////////////////////
//-- Defines

//#define DEBUG_DRAW 


///////////////////////////////////////////////////////////////////////////////
//-- Global variables

///////////////////////////////////////////////////////////////////////////////


const float COVERAGE[100] = { 0.2525314589560607, 0.2576326279264187, 0.2628360322702068, 0.2681431353083358, 0.2735553772432651, 0.2790741715891978, 0.2847009014014726, 0.2904369152983624, 0.2962835232704883, 0.3022419922728888, 0.308313541593817, 0.31449933799764, 0.3208004906361242, 0.3272180457262481, 0.3337529809911264, 0.3404061998622743, 0.3471785254421841, 0.3540706942267695,
0.3610833495887235, 0.3682170350233647, 0.3754721871601566, 0.3828491285443266, 0.3903480601940126, 0.3979690539407849, 0.4057120445616824, 0.4135768217143915, 0.42156302168711, 0.4296701189785898, 0.437897417723999, 0.4462440429868523, 0.4547089319362121, 0.463290824935085, 0.4719882565643009, 0.4807995466116144, 0.4897227910574863, 0.4987558530912262, 0.5078963541952966, 0.5171416653374513,
0.5264888983132224, 0.5359348972850012, 0.5454762305660166, 0.5551091827004417, 0.5648297468943986, 0.5746336178533678, 0.584516185086649, 0.5944725267394274, 0.6044974040160985, 0.6145852562605928, 0.6247301967608438, 0.6349260093447057, 0.64516614583707, 0.6554437244475375, 0.6657515291573695, 0.6760820101752764, 0.6864272855286133, 0.6967791438574159, 0.7071290484729112, 0.7174681427427703,
0.7277872568587884, 0.7380769160403831, 0.748327350219802, 0.7585285052521402, 0.7686700556833742, 0.7787414191045477, 0.7887317721106483, 0.7986300678752298, 0.8084250553400247, 0.8181053000119952, 0.8276592063450691, 0.8370750416769379, 0.846340961676199, 0.8554450372439244, 0.8643752828030637, 0.87311968589294, 0.8816662379765975, 0.890002966355447, 0.8981179670715378, 0.9059994386685738,
0.9136357166693593, 0.9210153086167311, 0.9281269295161807, 0.9349595375079411, 0.9415023695886816, 0.9477449771968978, 0.9536772614690937, 0.9592895079717747, 0.9645724207097471, 0.9695171552121149, 0.9741153504974227, 0.9783591597219075, 0.9822412793198499, 0.9857549764505119, 0.9888941145749449, 0.991653176994985, 0.9940272881980572, 0.9960122328654299, 0.9976044724143962, 0.9988011589619574,
0.9996001466136796, 1 };


/**
****************************************************************************************************
@brief Creates shadow map and other structures for selected light for multiresolution rendering
@param ii iterator to lights list
@return success/fail of shadow creation
****************************************************************************************************/
bool TScene::WarpedShadows_InitializeTechnique(vector<TLight*>::iterator ii)
{
    int sh_res = (*ii)->ShadowSize();
	GLuint texid, fbo, r_buffer, buffer;

	//-- Polynomial shadow technique
	//this->SetShadowTechnique( new PolynomialWarpedShadow() );

	//-- Bilinear shadow technique
	this->SetShadowTechnique( new BilinearWarpedShadow() );
	m_shadow_technique->SetResolution( 16.0 );

    //create data textures
    try{

		CreateDataTexture("MTEX_debug", sh_res, sh_res, GL_RGBA16F, GL_FLOAT);
        //cam coords
        CreateDataTexture("MTEX_coords", sh_res, sh_res, GL_RGBA32F, GL_FLOAT/*, GL_TEXTURE_2D, true*/);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        //output
        CreateDataTexture("MTEX_output", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT);
		CreateDataTexture("MTEX_mask", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT, GL_TEXTURE_2D, true);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		//blur
		CreateDataTexture("MTEX_ping", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		CreateDataTexture("MTEX_pong", sh_res/8, sh_res/8, GL_RGBA16F, GL_FLOAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//2D polynomials coefficients
		//FIXME: precision??? nestaci 16F ?
		CreateDataTexture("MTEX_2Dfunc_values", m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution(), GL_RGBA32F, GL_FLOAT);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
		//create renderbuffers
		glGenRenderbuffers(1, &m_aerr_r_buffer_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_aerr_r_buffer_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,sh_res, sh_res);

		glGenFramebuffers(1, &m_aerr_f_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_aerr_f_buffer);
    
		//attach texture to the frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["MTEX_coords"], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_aerr_r_buffer_depth);


		glGenTextures(1, (*ii)->GetShadowTexID());
		glBindTexture(GL_TEXTURE_2D_ARRAY, *(*ii)->GetShadowTexID());
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, sh_res, sh_res, 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		//FIXME: temporary color texture
		glGenTextures(1, &texid);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texid);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, sh_res, sh_res, 2, 0, GL_RGBA, GL_FLOAT, NULL);
        m_tex_cache["MTEX_warped_depth_color"] = texid;

		//glGenRenderbuffers(1, &r_buffer);
        //glBindRenderbuffer(GL_RENDERBUFFER, r_buffer);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,sh_res, sh_res);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  m_tex_cache["MTEX_warped_depth_color"], 0, 0);	    
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *(*ii)->GetShadowTexID(), 0, 0);	    
		//glDrawBuffer(GL_NONE); 
        //glReadBuffer(GL_NONE);
		m_fbos["fbo_warped_depth"] = fbo;

		//check FBO creation
        if(!CheckFBO())
        {
            ShowMessage("ERROR: FBO creation for depth map failed!",false);
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //add shaders handling multiresolution rendering

		//FIXME: debug shader
		AddMaterial("mat_debug_draw");
        CustomShader("mat_debug_draw","data/shaders/debug_draw.vert", "data/shaders/debug_draw.frag");

		// aliasing error
        AddMaterial("mat_camAndLightCoords_afterDP");
        CustomShader("mat_camAndLightCoords_afterDP","data/shaders/warping/camAndLightCoords_afterDP.vert", "data/shaders/warping/camAndLightCoords_afterDP.frag");

		//blur
		AddMaterial("mat_aliasblur_horiz",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
		AddTexture("mat_aliasblur_horiz","MTEX_output",RENDER_TEXTURE);
		CustomShader("mat_aliasblur_horiz","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ","#define HORIZONTAL\n");
		SetUniform("mat_aliasblur_horiz", "texsize", glm::ivec2(sh_res/8, sh_res/8));       //send texture size info

		AddMaterial("mat_aliasblur_vert",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
		AddTexture("mat_aliasblur_vert","MTEX_ping",RENDER_TEXTURE);
		CustomShader("mat_aliasblur_vert","data/shaders/quad.vert","data/shaders/warping/aliasblur.frag", " ", "#define VERTICAL\n");
		SetUniform("mat_aliasblur_vert", "texsize", glm::ivec2(sh_res/8, sh_res/8));       //send texture size info

		//alias gradient
		AddMaterial("mat_aliasgradient",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
		AddTexture("mat_aliasgradient","MTEX_pong",RENDER_TEXTURE);
		CustomShader("mat_aliasgradient","data/shaders/quad.vert","data/shaders/warping/aliasgradient.frag");

		//2D polynomial coefficients
		AddMaterial("mat_get_2Dfunc_values",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
		AddTexture("mat_get_2Dfunc_values","MTEX_ping",RENDER_TEXTURE);
		CustomShader("mat_get_2Dfunc_values","data/shaders/quad.vert","data/shaders/warping/get2DfuncValues.frag");

		//draw depth with warping
		AddMaterial("mat_depth_with_warping");
		//AddTexture("mat_depth_with_warping","MTEX_2Dfunc_values", RENDER_TEXTURE );
		CustomShader("mat_depth_with_warping","data/shaders/warping/drawDepthWithWarping.vert", "data/shaders/warping/drawDepthWithWarping.frag", m_shadow_technique->GetDefines(), "");

        //alias quad
        AddMaterial("mat_compute_aliasError",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
        AddTexture("mat_compute_aliasError","MTEX_coords",RENDER_TEXTURE);
        AddTexture("mat_compute_aliasError", "data/tex/error_color.tga");
        //AddTexture("mat_compute_aliasError","MTEX_coverage",RENDER_TEXTURE);
        CustomShader("mat_compute_aliasError","data/shaders/warping/computeAliasError.vert", "data/shaders/warping/computeAliasError.frag");

		//custom mipmap from mask
		AddMaterial("mat_mipmap_bb",white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
		AddTexture("mat_mipmap_bb","MTEX_mask",RENDER_TEXTURE);
		CustomShader("mat_mipmap_bb","data/shaders/quad.vert", "data/shaders/warping/mipmap_boundingbox.frag");
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
	lightViewMatrix[1] = glm::lookAt(l->GetPos(), l->GetPos() + glm::vec3(-m_far_p, 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );
	//glm::mat4 lightViewMatrix = glm::lookAt(l->GetPos(), glm::vec3( 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );

	//glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_matrices);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjMatrix));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 Mp = glm::mat4( 1.0 );
	Mp = glm::rotate( Mp, -1.0f*m_parab_angle.x, glm::vec3(1, 0, 0));
	Mp = glm::rotate( Mp, m_parab_angle.y, glm::vec3(0, 1, 0));

	lightViewMatrix[1] = Mp * lightViewMatrix[1];

	SetUniform("mat_camAndLightCoords_afterDP", "cam_mv", m_viewMatrix );
	SetUniform("mat_camAndLightCoords_afterDP", "cam_proj", m_projMatrix );
	SetUniform("mat_camAndLightCoords_afterDP", "lightMatrix", lightViewMatrix[1]); // FIXME: Bacha, je tady divna matice
	SetUniform("mat_camAndLightCoords_afterDP", "near_far", glm::vec2(SHADOW_NEAR, SHADOW_FAR));

	glm::mat4 coeffsX = glm::mat4( 0.0 );
	glm::mat4 coeffsY = glm::mat4( 0.0 );
	glm::vec4 mask_range = glm::vec4( 112, 16, 112, 16 );

	{
		glClearColor(99.0, 0.0, 0.0, 0.0);

		///////////////////////////////////////////////////////////////////////////////
		//-- 1. Render scene from light point of view and store per pixel camera 
		//--	coordinates and light coordinates

		glBindFramebuffer(GL_FRAMEBUFFER, m_aerr_f_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_coords"], 0);
		glDrawBuffers(1, mrt);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport( 0, 0, sh_res, sh_res );

		//if(m_wireframe)
		//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glEnable( GL_CLIP_PLANE0 );

		//glEnable( GL_CULL_FACE);
		glCullFace(GL_FRONT);

		//DrawSceneDepth("mat_aliasError", lightViewMatrix);
		DrawAliasError("mat_camAndLightCoords_afterDP", lightViewMatrix[1]);

		glViewport( 0, 0, sh_res, sh_res );
		//if(!m_wireframe)
		//	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

		glCullFace(GL_BACK);
		glDisable( GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		///////////////////////////////////////////////////////////////////////////////
		//-- 2. Compute alias error based on the coordinates data computed in step 1
		//--	Input: MTEX_coords (error texture)

		glBindFramebuffer(GL_FRAMEBUFFER, m_aerr_f_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_output"], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, m_tex_cache["MTEX_mask"], 0);
		glDrawBuffers(2, mrt);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport( 0, 0, sh_res/8, sh_res/8 );

		SetUniform("mat_compute_aliasError", "tex_coords", 0);
		SetUniform("mat_compute_aliasError", "tex_error", 1);
		//SetUniform("mat_compute_aliasError", "tex_coverage", 1); // musi byt 1, protoze tex_error je ze souboru (asi?)
		RenderPass("mat_compute_aliasError");

		glDrawBuffers(1, mrt);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, 0, 0);

		//FIXME: tenhle kod by mel prijit do PreRender metody, ale zatim neni vyresen pristup globalni pristup k texturam
		if ( (string) m_shadow_technique->GetName() ==  "Polynomial" )
		{
			//glBindTexture( GL_TEXTURE_2D, m_tex_cache["MTEX_mask"] ); 
			//glGenerateMipmap( GL_TEXTURE_2D );
			//glBindTexture( GL_TEXTURE_2D, 0 ); 


			float mask_values[128*128];
			glBindTexture(GL_TEXTURE_2D, m_tex_cache["MTEX_mask"]);
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
		
		//calculate custom mipmaps 
		/*
		int i,j;
		for(i=1, j = sh_res/8/2; j>=1; i++, j/=2)
		{
			glViewport(0, 0, j, j);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_cache["MTEX_mask"], i);
			SetUniform("mat_mipmap_bb", "mip_level", i-1);
			RenderPass("mat_mipmap_bb");
		}
		glViewport(0,0,sh_res/8,sh_res/8);  //restore viewport
		*/

		///////////////////////////////////////////////////////////////////////////////
		//-- 3. Blur the alias error
		//--	input: MTEX_ouput (horiz), MTEX_ping (vert)

		glDisable(GL_DEPTH_TEST);

		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_ping"], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		RenderPass("mat_aliasblur_horiz");
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_pong"], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		RenderPass("mat_aliasblur_vert");

		///////////////////////////////////////////////////////////////////////////////
		//-- 4. Compute gradient and store the result per-pixel into 128x128 texture

		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_ping"], 0);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::vec2 limit = glm::vec2(1.0);
		/*
		 * Tato cast kodu mela slouzit pro vypocet limitu posunuti. Prakticky se melo pousouvat o max polovinu velikosti bunky.
		 * Ale mira posunuti je ovlivnena i promeny POLY_BIAS a to se to pak komplikuje. Tak je to zatim u ledu.
		if( m_warping_enabled )
		{
			float w = (mask_range.y-mask_range.x);
			float h = (mask_range.w-mask_range.z);
			limit = glm::vec2( 1.0 );
		}
		*/
		SetUniform("mat_aliasgradient", "limit", limit );
		RenderPass("mat_aliasgradient");

		///////////////////////////////////////////////////////////////////////////////
		//-- 5. get a function value from gradient texture for a given grid (defined by 'range') and store it into 4x4 texture

		glm::vec4 func_range = m_shadow_technique->GetGrid()->GetRangeAsOriginStep();
			/*
			glm::vec4(
			mask_range.x,
			(mask_range.y-mask_range.x)/3.0f,
			mask_range.z,
			(mask_range.w-mask_range.z)/3.0f
			);
			*/
		 

		//func_range = glm::vec4( 0, 32, 0, 32 );

		glViewport( 0, 0, m_shadow_technique->GetResolution(), m_shadow_technique->GetResolution() );
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"], 0);

		glm::vec4 clear_color;
		glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( clear_color ) );
		glClearColor( 0, 0, 0, 0 );
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor( clear_color.r, clear_color.g, clear_color.b, clear_color.a );

		//glViewport( 0+1, 0+1, GRID_RES-2, GRID_RES-2 );
		if( m_shadow_technique->IsEnabled() )
		{
			SetUniform("mat_get_2Dfunc_values", "grid_res", (float) m_shadow_technique->GetResolution() );
			SetUniform("mat_get_2Dfunc_values", "range", func_range );
			RenderPass("mat_get_2Dfunc_values");
		}

		///////////////////////////////////////////////////////////////////////////////
		//-- 6. compute 2D polynomial coefficents and store them into textures (gradient for x and y axes)
		//-- odtud zacina cast, ktera je pro kazdou warpovaci metodu jina. Proto se pouziva abstraktni trida shadow_technique
		
		m_shadow_technique->SetTexId(m_tex_cache["MTEX_2Dfunc_values"]);
		m_shadow_technique->PreRender();

		///////////////////////////////////////////////////////////////////////////////

		glEnable(GL_DEPTH_TEST);
	}

	for(int i=0; i<2; i++)
    {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbos["fbo_warped_depth"]);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  m_tex_cache["MTEX_warped_depth_color"], 0, i);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *l->GetShadowTexID(), 0, i);	    
	//glDrawBuffers(1, mrt);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//-- Shared uniforms
	SetUniform("mat_depth_with_warping", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	SetUniform("mat_depth_with_warping", "range", mask_range);
	SetUniform("mat_depth_with_warping", "grid_res", (float) m_shadow_technique->GetResolution() );
	
	//-- Polynomial uniforms
	SetUniform("mat_depth_with_warping", "coeffsX", coeffsX );
	SetUniform("mat_depth_with_warping", "coeffsY", coeffsY );

	//-- Bilinear uniforms
	SetUniform("mat_depth_with_warping", "funcTex", 1 );
	

	    glCullFace(GL_FRONT);
        //glColorMask(0, 0, 0, 0);      //disable colorbuffer write
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport( 0, 0, sh_res, sh_res );
	glEnable(GL_CLIP_PLANE0);

	if(m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	float z_direction = 1.0;
	if(i == 1)
		z_direction = -1.0;  
	lightViewMatrix[i] = glm::lookAt(l->GetPos(), l->GetPos() + glm::vec3(m_far_p*z_direction, 0.0f, 0.0f ), glm::vec3(0.0f, 1.0f, 0.0f) );

	glm::mat4 Mp = glm::mat4( 1.0 );
	Mp = glm::rotate( Mp, z_direction*m_parab_angle.x, glm::vec3(1, 0, 0));
	Mp = glm::rotate( Mp, m_parab_angle.y, glm::vec3(0, 1, 0));

	lightViewMatrix[i] = Mp * lightViewMatrix[i];

	if(m_dpshadow_tess)
	{
		SetUniform("_mat_shadow_warp_tess", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
		SetUniform("_mat_shadow_warp_tess", "coeffsX", coeffsX );
		SetUniform("_mat_shadow_warp_tess", "coeffsY", coeffsY );

		DrawSceneDepth("_mat_shadow_warp_tess", lightViewMatrix[i]);
	}
	else
	{
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, m_tex_cache["MTEX_2Dfunc_values"] );
		DrawSceneDepth("mat_depth_with_warping", lightViewMatrix[i]);
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	if(!m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	}

	 //Finish, restore values
    glDisable( GL_CLIP_PLANE0 );
    glCullFace(GL_BACK);
    glColorMask(1, 1, 1, 1);

	glBindFramebuffer( GL_FRAMEBUFFER, 0);

	///////////////////////////////////////////////////////////////////////////////
	//-- DEBUG DRAW
#ifdef DEBUG_DRAW
	glBindFramebuffer(GL_FRAMEBUFFER, m_aerr_f_buffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_tex_cache["MTEX_debug"], 0);
	glDrawBuffers(1, mrt);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport( 0, 0, sh_res, sh_res );

	SetUniform("mat_debug_draw", "near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
	SetUniform("mat_debug_draw", "lightViewMatrix", lightViewMatrix[1]); // FIXME: Bacha, je tady divna matice

	glEnable(GL_CLIP_PLANE0);

	DrawAliasError("mat_debug_draw",m_viewMatrix);

	glDisable(GL_CLIP_PLANE0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		m_im->second->SetUniform("lightModelView[0]", lightViewMatrix[0]);
		m_im->second->SetUniform("lightModelView[1]", lightViewMatrix[1]);
		m_im->second->SetUniform("near_far_bias", glm::vec3(SHADOW_NEAR, SHADOW_FAR, POLY_BIAS));
		m_im->second->SetUniform("coeffsX", coeffsX );
		m_im->second->SetUniform("coeffsY", coeffsY );
		m_im->second->SetUniform("range", mask_range);
	}
}
