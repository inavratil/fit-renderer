#include "RenderListener.h"

#include "resources/SceneManager.h"

//-----------------------------------------------------------------------------

RenderListener::RenderListener( TScene* _scene ) :
m_scene( _scene )
{
	_Init();
}

//-----------------------------------------------------------------------------

RenderListener::~RenderListener(void)
{
	_Destroy();
}

//-----------------------------------------------------------------------------

void RenderListener::_Init()
{
	m_passes.clear();
}

//-----------------------------------------------------------------------------

void RenderListener::_Destroy()
{
	for(m_it_pass = m_passes.begin(); m_it_pass != m_passes.end(); m_it_pass++)
		delete m_it_pass->second;
	m_passes.clear(); 
}

//-----------------------------------------------------------------------------

void RenderListener::AppendPass( string _name, PassPtr _pass )
{
	if(m_passes.find(_name) != m_passes.end())
	{
		cerr<<"WARNING (AppendPass): pass with name "<<_name<<" already exist.\n";
		return;
	}
	if( _pass->Validate() )
	{
		_pass->SetQuad( SceneManager::Instance()->getVBO(VBO_ARRAY, "screen_quad") );
		m_passes[_name] = _pass;
	}
}

//-----------------------------------------------------------------------------

PassPtr RenderListener::GetPassPtr( string _name )
{
	if(m_passes.find(_name) == m_passes.end())
	{
		cerr<<"ERROR (GetPassPtr): no pass with name "<<_name<<" exist.\n";
		return NULL;
	}

	return m_passes[_name];
}

//-----------------------------------------------------------------------------

void RenderListener::SetShaderFeature( ShaderFeature* _pFeature )
{
	m_pShaderFeature = _pFeature;
}

//-----------------------------------------------------------------------------

ShaderFeature* RenderListener::GetShaderFeature()
{
	return m_pShaderFeature;
}

//-----------------------------------------------------------------------------
