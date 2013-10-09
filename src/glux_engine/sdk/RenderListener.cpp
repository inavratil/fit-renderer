#include "RenderListener.h"

//-----------------------------------------------------------------------------

RenderListener::RenderListener( TScene* _scene ) :
m_scene( _scene )
{
}

//-----------------------------------------------------------------------------

RenderListener::~RenderListener(void)
{
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
		_pass->SetQuad( m_screen_quad.vao );
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