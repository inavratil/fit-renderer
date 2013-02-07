#include "ScreenGrid.h"

ScreenGrid::ScreenGrid( float _res )
{
	_Init( glm::vec4(0.0, _res, 0.0, _res), _res );
}

ScreenGrid::ScreenGrid( glm::vec4 _range, float _res )
{
	_Init( _range, _res );
}


ScreenGrid::~ScreenGrid(void)
{
	delete m_pMat;
}

void ScreenGrid::_Init( glm::vec4 _range, float _res )
{
	m_iNumLines = 0; 
	m_iResolution = _res;

	UpdateRange( _range );

	TShader grid_vert("data/shaders/warping/dbg_polynomials_grid.vert", "");
	TShader grid_frag("data/shaders/warping/dbg_polynomials_grid.frag", "");
	
	m_pMat = NULL;
	m_pMat = new TMaterial("mat_drawScreenGrid", 0, white,white,white,0.0,0.0,0.0,SCREEN_SPACE);
	m_pMat->CustomShader(&grid_vert, NULL, NULL, NULL, &grid_frag);

	this->InitVertexData();
}

///////////////////////////////////////////////////////////////////////////////
//-- Public methods

void ScreenGrid::Draw()
{
	if(!m_pMat) return;

	m_pMat->RenderMaterial();
	glBindVertexArray(SceneManager::Instance()->getVBO(VBO_ARRAY, "polynomials_grid"));
	glDrawArrays( GL_LINES, 0, m_iNumLines );
	glBindVertexArray( 0 );
}

void ScreenGrid::InitVertexData()
{
	VBO tmp_vbo;

	glGenVertexArrays( 1, &tmp_vbo.vao );
	glBindVertexArray( tmp_vbo.vao );

	glGenBuffers( 2, tmp_vbo.buffer );
	glBindBuffer( GL_ARRAY_BUFFER, tmp_vbo.buffer[0] );
	glBufferData( GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW );
	glVertexAttribPointer(GLuint(0), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

	glBindBuffer( GL_ARRAY_BUFFER, tmp_vbo.buffer[1] );
	glBufferData( GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW );
	glVertexAttribPointer(GLuint(1), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

	SceneManager::Instance()->setVBO("polynomials_grid", tmp_vbo);

    glBindVertexArray(0);
}

void ScreenGrid::GenerateGrid( IShadowTechnique* _pShadowTech )
{
		vector<glm::vec2> vertices;
		vector<glm::vec3> colors;
		
		float nparts = 1.0; // kolik casti bude mezi dve body mrizky
		float eps = 1.0/(float)nparts;
		int res = m_iResolution; // rozliseni mrizky
		int K = 1; // kolik bunek bude mezi dvema ridicimi body
		int overlap = 0; // o kolik ridicich bodu mimo rozliseni se podivat

		float origin, major, minor;
		origin = 0 - overlap*K; // dolni hranice pro svisly i vodorovny smer, tedy pro s a t
		major = (res+overlap)*K-(K-1); // hranice pro hlavni smer pocitani, (K-1) je tam proto, abychom na konci meli vzdy posledni ridici bod 
		minor = major-1; // hranice pro vedlejsi smer pocitani
		// Pozn. hlavni vs. vedlejsi - pro rozliseni 4 (polynom) mame CTYRI cary ve TRECH radach
		//   ----------
		// 0 |  |  |  |
		//   ----------
		// 1 |  |  |  |
		//   ----------
		// 2 |  |  |  |
		//   ----------
		//   0  1  2  3
		//
		// tedy pro svisle cary bude s nabyvat hodnot 0..3 a t bude 0..2 (tedy s<res a t<res-1)

		m_iNumLines = 0;

		// 1/4 obrazu
		//glm::vec4 range = glm::vec4( -0.75, -0.25, 0.25, 0.75 );
		//range.x = range.x - 1.0;
		//range.y = range.y - 1.0;

		// 1/8 nahore
		//glm::vec4 range = glm::vec4( 41.0, 83.0, 11.0, 41.0 ) / 128.0;
		//range.x = range.x * 0.5 - 1.0;
		//range.y = range.y * 0.5 - 1.0;
		//range.z = range.z * 0.5 - 0.5;
		//range.w = range.w * 0.5 - 0.5;
		
		//FIXME: musi tam byt to +/- 1?
		//glm::vec4 range = (_grid.GetRange() + glm::vec4( -1.0, 1.0, -1.0, 1.0 )) / cTexRes;
		glm::vec4 range = m_vRange / m_cDefaultRes;
		range = range * 2.0 - 1.0;
		
		for( int t=origin; t<minor; ++t)
		for( int s=origin; s<major; ++s)
		for( int i=0;i<nparts;++i )
		{
			float x0, x1, y0, y1;
			
			// hodnoty v intervalu [0..3], resp. [0..res-1], kde res je rozliseni mrizky
			// (pro polynomy musi byt rozliseni 4
			x0 = s/(float)K; x1 = s/(float)K;
			y0 = (t+i*eps)/(float)K; y1 = (t+(i+1)*eps)/(float)K;
	
			glm::vec2 new_P(0.0), old_P(0.0);
			glm::vec2 diff(0.0);
	
			//-- Uprava souradnic podle polynomu
			diff = _pShadowTech->ComputeDiff( glm::vec2(x0, y0) );

			//-- prevod do intervalu [0..1], aby se diff mohl vztahovat k normalizovanym souradnicim
			old_P = glm::vec2(x0, y0) / (float)(res-1);
			//-- zde je nutny prevod z intervalu [0..1] do rozsahu mrizku (range)
			new_P = convertRange( old_P+diff, glm::vec4(0, 1, 0, 1), range );

			//-- ulozeni prvniho vrcholu cary
			vertices.push_back( new_P ); 
			if(s % K == 0)
				colors.push_back( glm::vec3(0.0,0.0,1.0) ); 
			else if( x0<0 || x0>(res-1) || y0<0 || y0>(res-1) ) 
				colors.push_back( glm::vec3(1.0,0.0,1.0) );
			else
				colors.push_back( glm::vec3(0.0,1.0,0.0) ); 
			m_iNumLines++;

			//-- Uprava souradnic podle polynomu
			diff = _pShadowTech->ComputeDiff( glm::vec2(x1, y1) );

			//-- prevod do intervalu [0..1], aby se diff mohl vztahovat k normalizovanym souradnicim
			old_P = glm::vec2(x1, y1) / (float)(res-1);
			//-- zde je nutny prevod z intervalu [0..1] do rozsahu mrizku (range)
			new_P = convertRange( old_P+diff, glm::vec4(0, 1, 0, 1), range );

			//-- ulozeni druheho vrcholu cary
			vertices.push_back( new_P ); 
			if(s % K == 0)
				colors.push_back( glm::vec3(0.0,0.0,1.0) ); 
			else if( x1<0 || x1>(res-1) || y1<0 || y1>(res-1) ) 
				colors.push_back( glm::vec3(1.0,0.0,1.0) );
			else
				colors.push_back( glm::vec3(0.0,1.0,0.0) ); 
			m_iNumLines++;
		}

		for( int s=origin; s<minor; ++s)
		for( int t=origin; t<major; ++t)
		for( int i=0;i<nparts;++i )
		{
			float x0, x1, y0, y1;
			x0 = (s+i*eps)/(float)K; x1 = (s+(i+1)*eps)/(float)K;
			y0 = t/(float)K; y1 = t/(float)K;
	
			glm::vec2 new_P(0.0), old_P(0.0);
			glm::vec2 diff(0.0);
	
			//-- Uprava souradnic podle polynomu
			diff = _pShadowTech->ComputeDiff( glm::vec2(x0, y0) );

			//-- prevod do intervalu [0..1], aby se diff mohl vztahovat k normalizovanym souradnicim
			old_P = glm::vec2(x0, y0) / (float)(res-1);
			//-- zde je nutny prevod z intervalu [0..1] do rozsahu mrizku (range)
			new_P = convertRange( old_P+diff, glm::vec4(0, 1, 0, 1), range );

			//-- ulozeni prvniho vrcholu cary
			vertices.push_back( new_P );
			if(t % K == 0)
				colors.push_back( glm::vec3(0.0,0.0,1.0) ); 
			else if( x0<0 || x0>(res-1) || y0<0 || y0>(res-1) ) 
				colors.push_back( glm::vec3(1.0,0.0,1.0) );
			else
				colors.push_back( glm::vec3(0.0,1.0,0.0) ); 
			m_iNumLines++;

			//-- Uprava souradnic podle polynomu
			diff = _pShadowTech->ComputeDiff( glm::vec2(x1, y1) );

			//-- prevod do intervalu [0..1], aby se diff mohl vztahovat k normalizovanym souradnicim
			old_P = glm::vec2(x1, y1) / (float)(res-1);
			//-- zde je nutny prevod z intervalu [0..1] do rozsahu mrizku (range)
			new_P = convertRange( old_P+diff, glm::vec4(0, 1, 0, 1), range );

			vertices.push_back( new_P );
			if(t % K == 0)
				colors.push_back( glm::vec3(0.0,0.0,1.0) ); 
			else if( x1<0 || x1>(res-1) || y1<0 || y1>(res-1) ) 
				colors.push_back( glm::vec3(1.0,0.0,1.0) );
			else
				colors.push_back( glm::vec3(0.0,1.0,0.0) ); 
			m_iNumLines++;
		}

		glBindBuffer(GL_ARRAY_BUFFER, SceneManager::Instance()->getVBO(VBO_BUFFER, "polynomials_grid", 0));
		glBufferData(GL_ARRAY_BUFFER, m_iNumLines * 2 * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);   //update vertex data
		glBindBuffer(GL_ARRAY_BUFFER, SceneManager::Instance()->getVBO(VBO_BUFFER, "polynomials_grid", 1));
		glBufferData(GL_ARRAY_BUFFER, m_iNumLines * 3 * sizeof(GLfloat), &colors[0], GL_DYNAMIC_DRAW);   //update color data
}