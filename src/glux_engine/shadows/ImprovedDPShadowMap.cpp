#include "ImprovedDPShadowMap.h"

#include "sdk/SimplePass.h"
#include "sdk/ScreenSpaceMaterial.h"

//-----------------------------------------------------------------------------

ImprovedDPShadowMap::ImprovedDPShadowMap( TScene* _scene ) :
DPShadowMap( _scene )
{
	_Init();
}

//-----------------------------------------------------------------------------

ImprovedDPShadowMap::~ImprovedDPShadowMap(void)
{
	_Destroy();
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::_Init()
{
	m_min_depth = 0.0;
	m_avg_depth = 100.0;
	m_max_depth = 1000.0;
	m_in_frustum = true;
	m_FOV = 0.0;
	m_select_buffer = NULL;

	m_light_position_eyespace = glm::vec3( 0.0f );
	m_avg_direction = glm::vec3( 0.0f );
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::_Destroy()
{
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::_PreDrawDepthMap()
{
	//-- Call parent method
	DPShadowMap::_PreDrawDepthMap();

	glm::mat4 cam_view_matrix = m_scene->GetViewMatrix();
	glm::mat4 cam_proj_matrix = m_scene->GetProjMatrix();

	//-- position of light in eye space
	m_light_position_eyespace =  glm::vec3(cam_view_matrix * glm::vec4(m_pLight->GetPos(), 1.0));

	//-- IPSM variables
	float cam_fovy = m_scene->GetCameraPtr()->GetFOVy();
	//size of near/far plane of view frustum
	//m_max_depth = 2*m_avg_depth;
	float far_hsize = m_max_depth * glm::tan(glm::radians(cam_fovy/2.0f));
	float near_hsize = m_min_depth * glm::tan(glm::radians(cam_fovy/2.0f));

	glm::vec3 cam_nearfar[2*F_POINTS];          //near/far points
	glm::vec3 nearfar_light[2*F_POINTS];        //vectors from near/far into light position in eye space
	///glm::vec3 look_point = glm::vec3( glm::inverse(cam_view_matrix) * glm::vec4(0.0f, 0.0f, 1.0, 1.0));


	//OPTIMAL VIEW FRUSTUM COVERAGE CALCULATION
	m_avg_direction = glm::vec3( 0.0 );
	m_FOV = 0.0;
	//Test, if light is inside frustum                
	glm::vec4 l_pos_proj =  glm::vec4(cam_proj_matrix * glm::vec4(m_light_position_eyespace, 1.0));
	l_pos_proj /= l_pos_proj.w;                
	if( l_pos_proj.x > -1.0 && l_pos_proj.x < 1.0 &&
		l_pos_proj.y > -1.0 && l_pos_proj.y < 1.0 &&
		l_pos_proj.z > -1.0 && l_pos_proj.z < 1.0 )
	{
		m_in_frustum = true;
		m_FOV = 179.0;        //we need to cover whole environment
	}
	else
	{
		m_in_frustum = false;
	}

	//position of far plane points in eye space                    
	cam_nearfar[0] = glm::vec3( far_hsize, -far_hsize, -m_max_depth);
	cam_nearfar[1] = glm::vec3( far_hsize,  far_hsize, -m_max_depth);
	cam_nearfar[2] = glm::vec3(-far_hsize,  far_hsize, -m_max_depth);
	cam_nearfar[3] = glm::vec3(-far_hsize, -far_hsize, -m_max_depth);
	//position of near plane points in eye space
	cam_nearfar[4] = glm::vec3( near_hsize, -near_hsize, -m_min_depth);
	cam_nearfar[5] = glm::vec3( near_hsize,  near_hsize, -m_min_depth);
	cam_nearfar[6] = glm::vec3(-near_hsize,  near_hsize, -m_min_depth);
	cam_nearfar[7] = glm::vec3(-near_hsize, -near_hsize, -m_min_depth);

	//vectors from near/far view frustum plane to light
	for(int j=0; j<2*F_POINTS; j++)
		nearfar_light[j] = glm::normalize(cam_nearfar[j] - m_light_position_eyespace);

	//Calculate avereage direction of vectors from light to frustum points
	for(int j=0; j<2*F_POINTS; j++)
		m_avg_direction = m_avg_direction + nearfar_light[j];
	m_avg_direction = glm::normalize(m_avg_direction);

	//Find highest angle between average direction and light-to-frustum vector
	for(int j=0; j<2*F_POINTS; ++j)
	{
		float deg = 2.0f * glm::degrees( glm::acos( glm::dot( m_avg_direction, nearfar_light[j] )));
		if( deg < 180.0f && deg > m_FOV )
			m_FOV = deg;
	}        

	m_zoom[0] = 1.0;
	m_zoom[1] = 1.0;
}

//-----------------------------------------------------------------------------

bool ImprovedDPShadowMap::_IsDrawingAllowed()
{
	return m_in_frustum;
}

//-----------------------------------------------------------------------------

glm::mat4 ImprovedDPShadowMap::_GetLightViewMatrix( int _i )
{	
	float z_direction = 1.0;
	if(_i == 1)
		z_direction = -1.0;  

	glm::mat4 cam_view_matrix = m_scene->GetViewMatrix();

	//looking point for light (for front paraboloid)
	glm::vec3 look_point = glm::vec3( glm::inverse(cam_view_matrix) * glm::vec4(z_direction*(m_avg_direction) 
		+ m_light_position_eyespace, 1.0f) ); 

	//TODO: when light is inside frustum it is better if paraboloid is aimed directly to camera position
	if(m_in_frustum)
		look_point = glm::vec3( z_direction * glm::inverse(cam_view_matrix) * glm::vec4(0.0f, 0.0f, -1.0, 1.0f) ); 

	//map 0-180 range of FOV into paraboloid zoom
	glm::vec2 dp_zoomcut = glm::vec2( glm::tan( glm::radians(m_FOV/2.0f) ), 1.0f );
	dp_zoomcut = glm::normalize( dp_zoomcut );
	m_zoom[_i] = dp_zoomcut.x/(dp_zoomcut.y + 1.0f);

	//-- Call parent method
	if( IsEnabled() )
		return glm::lookAt(
		m_pLight->GetPos(), 
		look_point, 
		glm::vec3(0.0f, 1.0f, 0.0f) );
	else
		return DPShadowMap::_GetLightViewMatrix( _i );
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::_UpdateShaderUniforms( int _i )
{
	//-- Call parent method
	DPShadowMap::_UpdateShaderUniforms( _i );

	Material* _mat_default_shadow_omni = m_scene->GetMaterialManager()->GetMaterial( "_mat_default_shadow_omni" );
	_mat_default_shadow_omni->SetUniform("ZOOM", m_zoom[_i] );

}

//-----------------------------------------------------------------------------
bool ImprovedDPShadowMap::Initialize()
{	
	try
	{
		//-- Call parent method	
		DPShadowMap::Initialize();

		//-- Get access to managers and caches
		TextureCachePtr texture_cache = m_scene->GetTextureCache();

		ScreenSpaceMaterial* mat = new ScreenSpaceMaterial( "mat_depth_select","data/shaders/quad.vert", "data/shaders/select_depth.frag" );
		mat->AddTexture( texture_cache->GetPtr( "normal_texture" ) );

		TexturePtr tex = texture_cache->Create2DManual( "select_texture", Z_SELECT_SIZE, Z_SELECT_SIZE, GL_RGBA16F, GL_FLOAT, GL_NEAREST, false );
		//-- pass
		SimplePassPtr pass_select = new SimplePass( Z_SELECT_SIZE, Z_SELECT_SIZE );
		pass_select->AttachOutputTexture( 0, tex );
		pass_select->SetShader( mat );
		this->AppendPass("pass_select", pass_select );	

		//allocate z-selection buffer
		m_select_buffer = new float[Z_SELECT_SIZE*Z_SELECT_SIZE];

	}
	catch( int )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::PreRender()
{
	//-- Call parent method
	DPShadowMap::PreRender();

	//-----------------------------------------------------------------------------
	//-- set light matrices and near/far planes to all materials
	MaterialManagerPtr material_manager = m_scene->GetMaterialManager();
	for(material_manager->Begin(); 
		!material_manager->End();
		material_manager->Next())
	{
		Material* mat = material_manager->GetItem();
		mat->SetUniform("ZOOM[0]", m_zoom[0] );
		mat->SetUniform("ZOOM[1]", m_zoom[1] );
	}

	MaterialPtr mat_aliasError = material_manager->GetMaterial( "mat_aliasError" );
	mat_aliasError->SetUniform("ZOOM[0]", m_zoom[0] );
	mat_aliasError->SetUniform("ZOOM[1]", m_zoom[1] );
}

//-----------------------------------------------------------------------------

void ImprovedDPShadowMap::PostRender()
{


	//GET CAMERA DISTANCE TO NEAREST OBJECT FROM Z-BUFFER
	this->GetPassPtr( "pass_select" )->Process();

	//copy depth values to buffer (from GPU to CPU)
	glBindTexture(GL_TEXTURE_2D, m_scene->GetTextureCache()->Get( "select_texture") );
	glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, m_select_buffer);
	//minimum, maximum and average depth
	float far_p = 10000;
	float near_p = 0.1;
	m_avg_depth = m_max_depth = 0.0;
	m_min_depth = 10000;
	int select_size = Z_SELECT_SIZE*Z_SELECT_SIZE;
	vector<float> sorted_buffer;

	//filter out values beyond far plane (where are no vertices) and push into vector for sorting
	for(int i=0; i<select_size; i++)
	{
		if(m_select_buffer[i] > 1.0 && m_select_buffer[i] < far_p)
			sorted_buffer.push_back(m_select_buffer[i]);
	}
	//sort by z-value
	std::sort(sorted_buffer.begin(), sorted_buffer.end());

	//run through 90% of pixels sorted by z-value
	select_size = int(0.9*sorted_buffer.size());
	for(int i=0; i<select_size; i++)
	{
		//depth values
		float curr_depth = sorted_buffer[i];
		if(curr_depth < m_min_depth)  //minimum
			m_min_depth = curr_depth;
		if(curr_depth > m_max_depth)  //maximum
			m_max_depth = curr_depth;
		m_avg_depth += curr_depth;    //average
	}
	m_avg_depth /= select_size;

	//cout<<"MIN: "<<m_min_depth<<" | AVG: "<<m_avg_depth<<" | MAX:"<<m_max_depth<<endl;

	//-- Call parent method
	DPShadowMap::PostRender();
}