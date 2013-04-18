#include "ConfigDialog.h"

//-----------------------------------------------------------------------------

ConfigDialog::ConfigDialog(void) :
	m_quit( 0 )
{
	Load();
	Setup();
}

//-----------------------------------------------------------------------------

ConfigDialog::~ConfigDialog(void)
{
	TwTerminate();
}

//-----------------------------------------------------------------------------

void ConfigDialog::Setup()
{
	TwInit( TW_OPENGL, NULL );
	TwWindowSize( 1024, 1024 );

	m_bar = TwNewBar( "Configuration" );

	TwAddVarRW( m_bar, "Quit", TW_TYPE_BOOL32, &m_quit, 
		" label='Quit?' true='+' false='-' key='ESC' help='Quit program.' ");

	TwDefine("Configuration refresh=0.1 position='40 40' ");
}

//-----------------------------------------------------------------------------

void ConfigDialog::Display()
{	
	SDL_Event event;
	int handled;

	while(!m_quit)
	{
		//int size[2];
		//TwGetParam(m_bar, NULL, "size", TW_PARAM_INT32, 2, size);
		glClear( GL_COLOR_BUFFER_BIT );
		TwDraw();
		// Present frame buffer
        SDL_GL_SwapBuffers();
		// Process incoming events
		while( SDL_PollEvent(&event) ) 
		{
			// Send event to AntTweakBar
			handled = TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);

		}
	}
}

//-----------------------------------------------------------------------------

void ConfigDialog::Load()
{
	ifstream cfg_file("config.cfg");
	if (cfg_file.is_open())
	{
		string line;
		while ( cfg_file.good() )
		{
			getline ( cfg_file, line);
			//-- ignore comments
			if (line.length() > 0 && line.at(0) != '#' )
			{
				cout << line << endl;
			}
		}
		cfg_file.close();
	}

	else 
		cerr << "WARNING: Unable to open file config file." << endl;


}

//-----------------------------------------------------------------------------
