#include "BilinearWarpedShadow.h"

#include "shadow.h"

BilinearWarpedShadow::BilinearWarpedShadow()
{
	_Init();
}


BilinearWarpedShadow::~BilinearWarpedShadow(void)
{
}

void BilinearWarpedShadow::_Init()
{
	m_sName = "Bilinear";
	m_pFuncValues = NULL;
}

bool BilinearWarpedShadow::Initialize(TLight* _light)
{
	return true;
}

void BilinearWarpedShadow::PreRender()
{
	int res = (int)this->GetResolution();
	float* z_values = new float[res*res*2];
	
	memset(z_values, 0, res*res*2*sizeof(float));

	glBindTexture(GL_TEXTURE_2D, m_iTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, z_values);
	glBindTexture(GL_TEXTURE_2D, 0 ); 

	m_pFuncValues = z_values;

	this->GenerateGrid();

	delete[] z_values;
	m_pFuncValues = NULL;
}

void BilinearWarpedShadow::PostRender()
{
}

//-- _P je v intervalu [0..res-1]
glm::vec2 BilinearWarpedShadow::ComputeDiff( glm::vec2 _P )
{
	glm::vec2 diff( 0.0 );

	if( m_iTexID == 0 || m_pFuncValues == NULL ) return diff;

	float res = this->GetResolution();
	//-- vypocet souradnice bunky, ve ktere se bod _P nachazi
	glm::vec2 grid_coords = glm::floor( _P );
	
	glm::vec2 temp, X, Y;
	glm::mat2 M;
	glm::vec4 f_values;

	//-- prevod do intervalu [0..1]
	float x = glm::fract(_P.x);
	float y = glm::fract(_P.y);

	X = glm::vec2(1-x,x);
	Y = glm::vec2(1-y,y);

	//-- diff X
	f_values = _GetFuncValuesFromCell( grid_coords, D_X );

	M = glm::mat2(f_values.x, f_values.y, f_values.z, f_values.w);

	temp = X * M;
	diff.x = glm::dot(temp, Y);

	//-- diff Y
	f_values = _GetFuncValuesFromCell( grid_coords, D_Y );

	M = glm::mat2(f_values.x, f_values.y, f_values.z, f_values.w);

	temp = X * M;
	diff.y = glm::dot(temp, Y);

	return diff;
}

#define I(i,j) (2*( (j)*(res) + (i) ))
glm::vec4 BilinearWarpedShadow::_GetFuncValuesFromCell( glm::vec2 _c, int _d )
{
	glm::vec4 ret( 0.0 );

	if( m_pFuncValues == NULL ) 
		return ret;

	int res = (int)this->GetResolution();
	int i = (int)_c.x, j = (int)_c.y;

	//-- jedna bunka
	//  C      D
	//   ------
	//   |    |
	//   |    |
	//   ------
	//  A      B

	ret.x = m_pFuncValues[I(i,j) + _d];		// A
	ret.y = m_pFuncValues[I(i+1,j) + _d];	// B
	ret.z = m_pFuncValues[I(i,j+1) + _d];	// C
	ret.w = m_pFuncValues[I(i+1,j+1) + _d]; // D

	return ret;
}
