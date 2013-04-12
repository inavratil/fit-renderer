#ifndef _PASS_H_
#define _PASS_H_

#include "resources/FBO.h"

class Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Pass(){};
	virtual ~Pass(void){};

//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Activate()		= 0;
	virtual void Deactivate()	= 0;
	virtual void Render()		= 0;

//-----------------------------------------------------------------------------
	void Process()
	{
		Activate();
		Render();
		Deactivate();
	}
};

typedef Pass* PassPtr;

#endif

