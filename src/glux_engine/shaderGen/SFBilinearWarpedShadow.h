#ifndef _SFBILINEARWARPEDSHADOW_H_
#define _SFBILINEARWARPEDSHADOW_H_

#include "ShaderFeature.h"

class SFBilinearWarpedShadow 
	: public ShaderFeature
{
protected:

public:
	SFBilinearWarpedShadow();
	~SFBilinearWarpedShadow();

	virtual void Init();
};

#endif
