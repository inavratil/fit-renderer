#ifndef _IMPROVEDDPSHADOWMAP_H_
#define _IMPROVEDDPSHADOWMAP_H_

#include "globals.h"
#include "DPShadowMap.h"

const int Z_SELECT_SIZE = 64;             //selection size of z-buffer (to compute average depth)

class ImprovedDPShadowMap : public DPShadowMap
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	float		m_min_depth, m_avg_depth, m_max_depth;
	glm::vec3	m_avg_direction;
	glm::vec3	m_light_position_eyespace;
	bool		m_in_frustum;
	float		m_FOV;

	float		m_zoom[2];
	float*		m_select_buffer;
	

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	ImprovedDPShadowMap( TScene* _scene );
	virtual ~ImprovedDPShadowMap(void);

	virtual bool Initialize();
	virtual void PreRender();
//	virtual void PostRender();

//-----------------------------------------------------------------------------
//-- Protected methods

protected:
	virtual void _PreDrawDepthMap();
	virtual glm::mat4 _GetLightViewMatrix( int _i );
	virtual void _UpdateShaderUniforms( int _i );

//-----------------------------------------------------------------------------
//-- Private methods

private:
	void _Init();
	void _Destroy();
};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

