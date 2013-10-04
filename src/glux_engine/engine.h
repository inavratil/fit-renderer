/**
****************************************************************************************************
****************************************************************************************************
@file: engine.h
@brief includes for engine
****************************************************************************************************
***************************************************************************************************/
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "globals.h"

//#include "timer.h"
#include "camera.h"
#include "compute.h"
#include "light.h"
#include "scene.h"
#include "texture.h"

//-- Resources
#include "resources/SceneManager.h"
#include "resources/TextureCache.h"

//-- Shadow techniques
#include "shadow.h"
#include "IWarpedShadowTechnique.h"

//-- SDK
#include "sdk/GeometryMaterial.h"
#include "sdk/Material.h"
#include "sdk/Pass.h"
#include "sdk/RenderListener.h"
#include "sdk/ScreenSpaceMaterial.h"
#include "sdk/SimplePass.h"



//-- Shader generation
#include "shaderGen/ShaderFeature.h"

//-- Miscellaenous
#include "ScreenGrid.h"


#endif
