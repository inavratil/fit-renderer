#include "SFBilinearWarpedShadow.h"

SFBilinearWarpedShadow::SFBilinearWarpedShadow()
{
	Init();
}

SFBilinearWarpedShadow::~SFBilinearWarpedShadow()
{
}

void SFBilinearWarpedShadow::Init()
{
	AddVariable("ObjSpacePosition", SG_VEC4, ShaderFeature::INOUT);
	ModifyVariable("ObjSpacePosition", SG_ASSIGN, SG_V4_VERTCOORD, ShaderFeature::VS);	//vertex object-space position
}

