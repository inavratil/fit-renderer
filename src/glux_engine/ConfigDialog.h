#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include "globals.h"

typedef	map<string,string> Options;
typedef map<string,Options*> SettingsMap;	

class ConfigDialog
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	TwBar*	m_bar;
	bool	m_is_initialized;
	int		m_quit;

	Options::iterator			m_it_options;	
	SettingsMap					m_settings;
	SettingsMap::iterator		m_it_setings;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	ConfigDialog(void);
	virtual ~ConfigDialog(void);

	void Setup();
	void Display();
	void Load( const string& _filename );

	//int GetInteger( string _name )
	//{ 
	//	stringstream s(m_settings[_name]);
	//	int num;
	//	s >> num;
	//	return num;
	//}
	//bool GetBoolean( string _name )
	//{ 
	//	return ( m_settings[_name]=="Yes" ||  m_settings[_name]=="yes" );
	//}
	//string GetString( string _name )
	//{ 
	//	return m_settings[_name];
	//}
	
	//-- prevzato z OGRE Ogre::ConfigFile::getSetting(...)
	string GetSetting(const string& key, const string& section = "", const string& defaultValue = "") const
    {
        
        SettingsMap::const_iterator c_it_setings = m_settings.find(section);
        if (c_it_setings == m_settings.end())
        {
            return defaultValue;
        }
        else
        {
            Options::const_iterator i = c_it_setings->second->find(key);
            if (i == c_it_setings->second->end())
            {
                return defaultValue;
            }
            else
            {
                return i->second;
            }
		}
	}
       
};

typedef ConfigDialog* ConfigDialogPtr;


#include <cctype>

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

#endif

