#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "sdk/Singleton.h"

template< typename T > class Resource : public Singleton<T>
{ 

//-----------------------------------------------------------------------------
// -- Member variables

protected:

//-----------------------------------------------------------------------------
//-- Public methods 

public:
	Resource(void);
	virtual ~Resource(void);

//-----------------------------------------------------------------------------
//-- Private methods

private:

};

//-----------------------------------------------------------------------------
//-- Inline methods 

#endif

