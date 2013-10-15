#ifndef _DPSMAPP_H_
#define _DPSMAPP_H_

#include "sdk/Application.h"

class DPSMApp : public Application
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	DPSMApp(void);
	virtual ~DPSMApp(void);

	/*virtual*/ void InitGUI();
	/*virtual*/ void CreateContent();	// <== Must be redefined
	/*virtual*/ void UpdateScene();
//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

