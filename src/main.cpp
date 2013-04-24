#include "main_ui.h"

#include "IPSMApp.h"

//*****************************************************************************
//Initialize OpenGL settings for scene
void IPSMApp::CreateContent()
{ 	
	const char *cubemap[] = {   "data/tex/cubemaps/posx.tga", "data/tex/cubemaps/negx.tga",
		"data/tex/cubemaps/posy.tga", "data/tex/cubemaps/negy.tga",
		"data/tex/cubemaps/posz.tga", "data/tex/cubemaps/negz.tga" };

	bool use_pcf = false;
	bool use_tess = false;
	int shadow_res = 1024;
	int dpshadow_method = WARP_DPSM;
	glm::vec3 parab_rot = glm::vec3( 0 );

	//-------------------------------------------------------------------------

	m_scene->AddLight(0, dgrey, white, white, glm::vec3(0.0,0.0,0.0), 1000.0f);

	int scene = SetupExperiments( "experiments.cfg" );
	//skybox
	//s->AddMaterial("mat_sky",white,white,white,0.0,0.0,0.0,NONE);
	//s->AddTexture("mat_sky",cubemap);
	//s->AddObject("sky",CUBE,5000.0,5000.0);
	//s->SetMaterial("sky","mat_sky");
	//cast/receive shadows
	//s->ObjCastShadow("sky",false);
	//s->MatReceiveShadow("mat_sky",false);

	//s->AddObject("camera",CUBE, 10, 10);//"data/obj/camera.3ds");
	//s->AddMaterial("mat_camera", black, green);
	//s->SetMaterial("camera", "mat_camera");

	//scene 1 - car
	if(scene == 1)
	{            
		m_scene->LoadScene("data/obj/scenes/car.3ds");
		//glass
		//FIXME: nastavit v LoadScene: s->SetTransparency("sklo",0.7f);

		//s->AddTexture("pletivo", "data/tex/alpha/fence.tga",ALPHA);
		//s->AddTexture("body",cubemap, CUBEMAP_ENV, ADD, 0.4f);
		//s->AddTexture("sklo",cubemap, CUBEMAP_ENV, ADD, 0.4f);
		//s->AddTexture("zem",cubemap, CUBEMAP_ENV, ADD, 0.2f);
		//s->AddTexture("chrom",cubemap, CUBEMAP_ENV, ADD, 0.2f);
		//s->AddTexture("zrcatko",cubemap, CUBEMAP_ENV, ADD, 0.2f);
		//s->AddTexture("cihle","data/tex/depth/bricks.tga",PARALLAX,MODULATE,0.0001f);
	}
	//scene 2 - cathedral
	else if(scene == 2)
	{
		//m_scene->LoadScene("data/obj/scenes/sibenik.3ds");
		m_scene->LoadScene("data/obj/scenes/testing_tess_all.3ds");
	}
	//scene 3 - bad scene
	else if(scene == 3)
	{
		//s->LoadScene("../data/obj/scenes/zla_scena.3ds");

		//s->AddObject("ground",PLANE,1000.0,1000.0);
		m_scene->AddObject("ground","data/obj/plane.3ds");            
		m_scene->AddObject("cube",CUBE,10.0,10.0);
		m_scene->MoveObj("cube", 100.0, 10.0, 100.0 );
		m_scene->SetMaterial("cube","mat_green");

		//add materials
		m_scene->AddMaterial("mat_ground",silver,silver);
		m_scene->AddMaterial("mat_cone",green,white);            
		//set materials
		m_scene->SetMaterial("ground","mat_ground");

		int CONE_COUNT = 10;
		string cone;

		for(int i=0; i<CONE_COUNT; ++i)
		{
			cone = "cone" + num2str(i);
			m_scene->AddObject(cone.c_str(), "data/obj/cone.3ds");

			float movx = (i%5)*50.0f - 10.0f*CONE_COUNT;
			float movz = (i/5)*200.0f - 100.0f;

			m_scene->MoveObj(cone.c_str(), movx, 0.0, movz );                
			m_scene->SetMaterial(cone.c_str(),"mat_cone");
		}           
	}
	//scene 4 - outdoor
	else if(scene == 4)
	{
		m_scene->AddObject("ground",PLANE,1000.0,1000.0);

		//add materials
		m_scene->AddMaterial("mat_bark",lgrey,white);
		m_scene->AddMaterial("mat_leaf",lgrey,white);
		m_scene->AddMaterial("mat_ground",silver,silver);

		//add textures
		//s->AddTexture("mat_bark","data/tex/bark4.tga");
		//s->AddTexture("mat_bark","data/tex/normal/bark4_nm.tga",BUMP,MODULATE, 1.0);
		//s->AddTexture("mat_leaf","data/tex/alpha/leafs4.tga",ALPHA);
		//s->AddTexture("mat_ground","data/tex/grass.tga",BASE, MODULATE, 1.0, 50.0, 100.0);

		//set materials
		m_scene->SetMaterial("ground","mat_ground");

		//trees
		float rnd;
		int OBJECT_COUNT = 7;
		string bark, leaf;
		for(int i=0; i<OBJECT_COUNT; i++)
		{
			for(int j=0; j<OBJECT_COUNT; j++)
			{
				rnd = rand()%100 / 25.0f - 2.0f;
				bark = "tree" + num2str(i*OBJECT_COUNT + j) + "_bark";
				leaf = "tree" + num2str(i*OBJECT_COUNT + j) + "_leaf";
				//two tree types
				if( (i*OBJECT_COUNT + j)%2 == 1)
				{
					m_scene->AddObject(bark.c_str(),"data/obj/trees/tree1_bark.3ds");
					m_scene->AddObject(leaf.c_str(),"data/obj/trees/tree1_leaf.3ds");
				}
				else
				{
					m_scene->AddObject(bark.c_str(),"data/obj/trees/tree2_bark.3ds");
					m_scene->AddObject(leaf.c_str(),"data/obj/trees/tree2_leaf.3ds");
				}
				float movx = j*50.0f + rnd - 25.0f*OBJECT_COUNT;
				float movz = i*50.0f - rnd - 25.0f* OBJECT_COUNT;
				float roty = 10.0f * rnd;
				m_scene->MoveObj(bark.c_str(), movx, 0.0, movz);
				m_scene->MoveObj(leaf.c_str(), movx, 0.0, movz);
				m_scene->RotateObj(bark.c_str(), roty, A_Z);
				m_scene->RotateObj(leaf.c_str(), roty, A_Z);
				m_scene->SetMaterial(bark.c_str(), "mat_bark");
				m_scene->SetMaterial(leaf.c_str(), "mat_leaf");
			}
		}

		//big tree
		m_scene->AddObject("bigtree_bark","data/obj/trees/tree2_bark.3ds");
		m_scene->ResizeObj("bigtree_bark", 10.0,10.0,10.0);
		m_scene->AddObject("bigtree_leaves","data/obj/trees/tree2_leaf.3ds");
		m_scene->ResizeObj("bigtree_leaves", 10.0,10.0,10.0);
		m_scene->SetMaterial("bigtree_bark","mat_bark");
		m_scene->SetMaterial("bigtree_leaves","mat_leaf");
	}      


	//---------------------------------------------------------------------

#ifdef USE_DP        
	//dual-paraboloid shadow parameters        
	m_scene->SetShadow(0, shadow_res, OMNI, 0.3f, true);
	m_scene->DPSetPCF( use_pcf );
	//s->DPSetCutAngle( m_param_cut_angle );
	m_scene->DPShadowMethod( dpshadow_method );         
	m_scene->DPSetTessellation( use_tess );
	m_scene->RotateParaboloid( parab_rot );
#else
	s->SetShadow(0, SHADOW_RES, SPOT, 0.3f);
#endif

	//---------------------------------------------------------------------

	//toggle effects (HDR & SSAO)
	//s->UseHDR();
	//s->UseSSAO();

	//prepare render to texture
	//s->CreateHDRRenderTarget(-1, -1, GL_RGBA16F, GL_FLOAT);

	////General HDR settings
	//s->SetUniform("mat_tonemap","exposure",1.0);
	//s->SetUniform("mat_tonemap","bloomFactor",0.3);
	//s->SetUniform("mat_tonemap","brightMax",1.7);
	//s->SetUniform("mat_bloom_hdr_ssao","THRESHOLD",0.5);
	////SSAO settings
	//s->SetUniform("mat_bloom_hdr_ssao","intensity",0.7);
	//s->SetUniform("mat_bloom_hdr_ssao","radius",0.004);
	//s->SetUniform("mat_bloom_hdr_ssao","bias",1.5);

	//---------------------------------------------------------------------

	m_scene->AddMaterial( new ScreenSpaceMaterial( "show_depth", "data/shaders/showDepth.vert", "data/shaders/showDepth.frag" ) );

	//draw quad
	m_scene->AddMaterial( new ScreenSpaceMaterial( "mat_quad","data/shaders/quad.vert", "data/shaders/quad.frag" ) );
	m_scene->AddMaterial( new ScreenSpaceMaterial( "mat_quad_lod","data/shaders/quad.vert", "data/shaders/quad_lod.frag" ) );
	m_scene->AddMaterial( new ScreenSpaceMaterial( "mat_quad_array","data/shaders/quad.vert", "data/shaders/quad_array.frag" ) );        

	//add shadow shader when shadows are enabled (will be sending depth values only)
	m_scene->AddMaterial( new ScreenSpaceMaterial( "_mat_default_shadow", "data/shaders/shadow.vert", "data/shaders/shadow.frag" ) );
	string defines;
	//if(dpshadow_method == CUT)
	//	defines = "#define PARABOLA_CUT\n";

	//and also for omnidirectional lights with dual-paraboloid
	m_scene->AddMaterial( new ScreenSpaceMaterial( "_mat_default_shadow_omni", "data/shaders/shadow_omni.vert", "data/shaders/shadow_omni.frag" ) );

	//optionally, add tessellation for paraboloid projection
	/*
	if(s->DPGetTessellation())
	{        
	TShader vert("data/shaders/shadow_omni_tess.vert", "");
	TShader tcon("data/shaders/shadow_omni_tess.tc", "");
	TShader teval("data/shaders/shadow_omni_tess.te", "");
	TShader frag("data/shaders/shadow_omni_tess.frag", "");

	s->AddMaterial("_mat_default_shadow_omni_tess", white, white, white, 64.0, 0.0, 0.0, NONE);
	s->CustomShader("_mat_default_shadow_omni_tess", &vert, &tcon, &teval, NULL, &frag);

	TShader warp_vert("data/shaders/shadow_warp_tess.vert", "");
	TShader warp_tcon("data/shaders/shadow_warp_tess.tc", "");
	TShader warp_teval("data/shaders/shadow_warp_tess.te", "");
	TShader warp_frag("data/shaders/shadow_warp_tess.frag", "");

	s->AddMaterial("_mat_shadow_warp_tess", white, white, white, 64.0, 0.0, 0.0, NONE);
	s->CustomShader("_mat_shadow_warp_tess", &warp_vert, &warp_tcon, &warp_teval, NULL, &warp_frag);
	}
	*/

	//-------------------------------------------------------------------------
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//********************************* MAIN ********************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	IPSMApp app;

	return app.Run();
}
