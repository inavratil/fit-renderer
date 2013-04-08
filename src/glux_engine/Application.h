#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "globals.h"

class Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Application(void);
	virtual ~Application(void);

	int Run();

//-----------------------------------------------------------------------------
//-- Private methods

private: 
	void _Destroy();


};

#endif

