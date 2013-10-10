#include "IWarpedShadowTechnique.h"

//-----------------------------------------------------------------------------

IWarpedShadowTechnique::IWarpedShadowTechnique( TScene* _scene ) :
	IShadowTechnique( _scene )
{
	_Init();
}

//-----------------------------------------------------------------------------

IWarpedShadowTechnique::~IWarpedShadowTechnique(void)
{
	_Destroy();
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::_Init()
{
	m_iTexID = 0;
	m_pScreenGrid = new ScreenGrid( m_cDefaultRes );
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::_Destroy()
{
	delete m_pScreenGrid;
	delete m_pShaderFeature;
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::SetControlPointsCount( float _res )
{
	m_pScreenGrid->SetControlPointsCount( _res ); 
}

//-----------------------------------------------------------------------------

float IWarpedShadowTechnique::GetControlPointsCount()
{
	return m_pScreenGrid->GetControlPointsCount();
}

//-----------------------------------------------------------------------------

const char* IWarpedShadowTechnique::GetName()
{
	return m_sName;
}

//-----------------------------------------------------------------------------

const char* IWarpedShadowTechnique::GetDefines()
{
	return m_sDefines;
}

//-----------------------------------------------------------------------------

GLuint IWarpedShadowTechnique::GetTexId()
{ 
	return m_iTexID; 
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::SetTexId( GLuint _texid)
{ 
	m_iTexID = _texid; 
}

//-----------------------------------------------------------------------------

ScreenGrid* IWarpedShadowTechnique::GetGrid()
{
	return m_pScreenGrid;
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::SetShaderFeature( ShaderFeature* _pFeature )
{
	m_pShaderFeature = _pFeature;
}

//-----------------------------------------------------------------------------

ShaderFeature* IWarpedShadowTechnique::GetShaderFeature()
{
	return m_pShaderFeature;
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::UpdateGridRange( glm::vec4 _range )
{
	m_pScreenGrid->UpdateRange(_range);
}

//-----------------------------------------------------------------------------

glm::vec4 IWarpedShadowTechnique::GetGridRange()
{
	return m_pScreenGrid->GetRange();
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::GenerateGrid()
{
	m_pScreenGrid->GenerateGrid(this);
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::DrawGrid()
{
	m_pScreenGrid->Draw();
}

//-----------------------------------------------------------------------------

void IWarpedShadowTechnique::ModifyGrid(glm::vec4 *precomputed_diffs)
{
	const int res = 16;
	float *data = new float[res*res];

	glBindTexture( GL_TEXTURE_2D, TextureCache::Instance()->Get("aliaserr_mipmap" ));
	glGetTexImage( GL_TEXTURE_2D, 3, GL_ALPHA, GL_FLOAT, data);
	glBindTexture( GL_TEXTURE_2D, 0);

	for(int i=0; i<res*res; ++i )
		if( data[i] == 0.0 ) 
			data[i] = 1.0;
	
	//for( int j=0; j<res; j++  )
	//{
	//	for( int i=0; i<res; i++  )
	//	{
	//		cout << data[j*res + i] << ", ";
	//	}
	//	cout << endl;
	//}
	//
	//cout << "--------" << endl;

	glm::vec4 borders[res*res];

	for( int y=0; y<res; ++y )
		for( int x=0; x<res; ++x )
		{
			int s,t;

			//top
			s = glm::clamp(x,0,15); 
			t = glm::clamp(y+1,0,15);

			borders[y*res+x].x = glm::max(data[t*res+s], data[y*res+x]);
			//borders[y*res+x].x = (data[t*res+s]+ data[y*res+x])*0.5f;

			//right
			s = glm::clamp(x+1,0,15); 
			t = glm::clamp(y,0,15);

			borders[y*res+x].y = glm::max(data[t*res+s], data[y*res+x]);
			//borders[y*res+x].y = (data[t*res+s], data[y*res+x])*0.5f;

			//bottom
			s = glm::clamp(x,0,15); 
			t = glm::clamp(y-1,0,15);

			borders[y*res+x].z = glm::max(data[t*res+s], data[y*res+x]);
			//borders[y*res+x].z = 0.5f*(data[t*res+s], data[y*res+x]);

			//left
			s = glm::clamp(x-1,0,15); 
			t = glm::clamp(y,0,15);

			borders[y*res+x].w = glm::max(data[t*res+s], data[y*res+x]);
			//borders[y*res+x].w = 0.5f*(data[t*res+s], data[y*res+x]);
		}

	delete data;

	//cout << "Borders: " << endl;
	//for( int y=0; y<res; ++y )
	//{
	//    for( int x=0; x<res; ++x )
	//    {
	//        glm::vec4 v = borders[y*res + x];
	//        cout << v.z << ", " << "\t";
	//    }
	//    cout << endl;
	//}

	float horizSum[res+1];
	float vertSum[res+1];

	memset(horizSum, 0, (res+1)*sizeof(float));
	memset(vertSum, 0, (res+1)*sizeof(float));

	for( int y=0; y<res; ++y )
		for( int x=0; x<res; ++x )
		{
			glm::vec4 v = borders[y*res+x];
			horizSum[y] += v.z;
			if(y==(res-1))
				horizSum[y+1] += v.x;
			vertSum[x] += v.w;
			if(x == (res-1))
				vertSum[x+1] += v.y;
		}

	//for( int i=0; i<(res+1); ++i )
	//    cout << horizSum[i] << endl;
	//cout<<"---------------"<<endl;
	//for( int i=0; i<(res+1); ++i )
	//    cout << vertSum[i] << endl;

	for( int y=0; y<res; ++y )
		for( int x=0; x<res; ++x )
		{
			if( horizSum[y+1] != 0 ) borders[y*res+x].x /= horizSum[y+1];
			if( horizSum[y] != 0 ) borders[y*res+x].z /= horizSum[y];
			if( vertSum[x+1] != 0 ) borders[y*res+x].y /= vertSum[x+1];
			if( vertSum[x] != 0 ) borders[y*res+x].w /= vertSum[x];
		}
	//cout << "Normalized Borders: " << endl;
	//for( int y=0; y<res; ++y )
	//{
	//    for( int x=0; x<res; ++x )
	//    {
	//        glm::vec4 v = borders[y*res + x];
	//        cout << v.z << ", " << "\t";
	//    }
	//    cout << endl;
	//}

	float step = 1.0/16.0;
	const int d = (res+1);
	glm::vec4 diffs[d*d];
	memset(diffs, 0, d*d*sizeof(glm::vec4));

	for( int y=0; y<d; ++y )
		for( int x=0; x<d; ++x )
		{
			glm::vec4 v = borders[y*res+x];
			float dx, dy, lastX, lastY;
			dx = dy = lastX = lastY = 0.0;

			if(x>0)
			{
				if(y==res)
					lastX = diffs[y*d+(x-1)].z + borders[(y-1)*res+(x-1)].x; // top
				else
					lastX = diffs[y*d+(x-1)].z + borders[y*res+(x-1)].z; // bottom
				dx = lastX - x*step;
			}
			if(y>0)
			{
				if(x==res)
					lastY = diffs[(y-1)*d+x].w + borders[(y-1)*res+(x-1)].y; // right
				else
					lastY = diffs[(y-1)*d+x].w + borders[(y-1)*res+x].w; // left
				dy = lastY - y*step;
			}

			diffs[y*d+x] = glm::vec4(dx,dy,lastX,lastY);

		}

	//cout << "Diffs: " << endl;

	for( int y=0; y<d; ++y )
	{
		for( int x=0; x<d; ++x )
		{
			glm::vec4 v = diffs[y*d + x];

			precomputed_diffs[(y+1)*19 + (x+1)] = glm::vec4( v.x, v.y, 0.0f, 0.0f ); 
		}
	}

}

//-----------------------------------------------------------------------------