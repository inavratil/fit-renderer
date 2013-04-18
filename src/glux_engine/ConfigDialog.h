#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include "globals.h"

class ConfigDialog
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	TwBar*	m_bar;
	int		m_quit;

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	ConfigDialog(void);
	virtual ~ConfigDialog(void);

	void Setup();
	void Display();
	void Load();
};

#endif

