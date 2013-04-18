#ifndef _IPSMAPP_H_
#define _IPSMAPP_H_

#include "globals.h"
#include "glux_engine/Application.h"

class IPSMApp : public Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	IPSMApp(void);
	virtual ~IPSMApp(void);

	virtual void CreateContent();
//-----------------------------------------------------------------------------
//-- Private methods

private: 


};

#endif

