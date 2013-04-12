#ifndef _PASS_H_
#define _PASS_H_

#include "resources/FBO.h"

class Pass
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:
	bool					m_valid;
//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Pass();
	virtual ~Pass(void);

//-----------------------------------------------------------------------------
	//-- Virtual Methods
	virtual void Activate()		= 0;
	virtual void Deactivate()	= 0;
	virtual void Render()		= 0;

	virtual bool Validate()		= 0;

//-----------------------------------------------------------------------------
	void Process()
	{
		Activate();
		Render();
		Deactivate();
	}

	bool IsValid(){ return m_valid; }
};

typedef Pass* PassPtr;

#endif

