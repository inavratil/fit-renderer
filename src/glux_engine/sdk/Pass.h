#ifndef _PASS_H_
#define _PASS_H_

#include "resources/FBO.h"
#include "Material.h"

class Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	bool					m_valid;
	MaterialPtr				m_material;
	GLuint					m_quad_vao;
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Pass();
	virtual ~Pass(void);

//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual bool Validate()		= 0;
	virtual void Activate()		= 0;
	virtual void Deactivate()	= 0;	

	virtual void Render();

//-----------------------------------------------------------------------------
	void Process()
	{
		Activate();
		Render();
		Deactivate();
	}

	bool IsValid(){ return m_valid; }
	void SetShader( const MaterialPtr _material ){ m_material = _material; }
	MaterialPtr GetShader()
	{
		if( !m_material )
			cerr << "WARNING (Pass): material is null." << endl;
		return m_material;
	}

	void SetQuad( const GLuint _vao ){ m_quad_vao = _vao; } //FIXME: tohle jinak
};

typedef Pass* PassPtr;

#endif

