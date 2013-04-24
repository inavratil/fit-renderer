#include "ConfigDialog.h"

//-----------------------------------------------------------------------------

ConfigDialog::ConfigDialog(void) :
	m_quit( 0 ),
	m_is_initialized( false )
{
}

//-----------------------------------------------------------------------------

ConfigDialog::~ConfigDialog(void)
{
	TwTerminate();
}

//-----------------------------------------------------------------------------

void ConfigDialog::Setup()
{
	if( m_is_initialized ) return;

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
	Setup();

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
			if( !handled )
				if( event.type == SDL_QUIT )
					m_quit = 1;
		}
	}
}

//-----------------------------------------------------------------------------

void ConfigDialog::Load( const string& _filename )
{
	ifstream cfg_file( _filename );
	if (cfg_file.is_open())
	{
		string line;
		string section;
		//-- insert empty section, for safety reasons
		Options* options = new Options();
		m_settings[section] = options;

		while ( cfg_file.good() )
		{
			getline ( cfg_file, line);
			
			//-- inspired by Ogre::OgreConfigFile::load(...)
			//-- ignore comments and whitespaces
			if (line.length() > 0 && line.at(0) != '#' )
			{
				if ( line.at(0) == '[' && line.at(line.length()-1) == ']' )
				{
					section = line.substr( 1, line.length()-2 );
					m_it_setings = m_settings.find( section );
					if( m_it_setings == m_settings.end() )
					{
						options = new Options();
						m_settings[section] = options;
					}
					else
						options = m_it_setings->second;
				}
				else
				{
					string::size_type sep_pos = line.find_first_of("=");
					if( sep_pos != string::npos )
					{
						string name = line.substr(0, sep_pos );

						string::size_type nonsep_pos = line.find_first_not_of("=", sep_pos);
						string value  = line.substr(nonsep_pos);

						options->insert( Options::value_type( trim(name), trim(value) ) );
					}
				}
			}
		}
		cfg_file.close();
	}

	else 
		cerr << "WARNING: Unable to open file config file." << endl;


}

//-----------------------------------------------------------------------------
