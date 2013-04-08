#ifndef _BLITPASS_H_
#define _BLITPASS_H_

#include "globals.h"
#include "Pass.h"

class BlitPass : public Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	GLuint		m_tex_read;
	GLuint		m_tex_draw;

	GLuint		m_fbo_read;
	GLuint		m_fbo_draw;

	GLuint		m_mask;
	GLuint		m_filter;

	glm::ivec4	m_src_bounds;
	glm::ivec4	m_dst_bounds;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	BlitPass(GLuint _input_tex, GLuint _output_tex);
	virtual ~BlitPass(void);

	virtual void Render();

	//-- Set/Get Filter
	void SetFilter( GLuint _filter ){ m_filter = _filter; };
	GLuint GetFilter(){ return m_filter; };

	//-- Set/Get Mask
	void SetMask( GLuint _mask ){ m_mask = _mask; };
	GLuint GetMask(){ return m_mask; };

	//-- Set/Get source bounds
	void SetSrcBounds( glm::ivec4 _bounds ){ m_src_bounds = _bounds; };
	glm::ivec4 GetSrcBounds(){ return m_src_bounds; };

	//-- Set/Get destination bounds
	void SetDstBounds( glm::ivec4 _bounds ){ m_dst_bounds = _bounds; };
	glm::ivec4 GetDstBounds(){ return m_dst_bounds; };

//-----------------------------------------------------------------------------
//-- Private methods 

private:
	void _InitBounds();
};

#endif

