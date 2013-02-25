/**
****************************************************************************************************
****************************************************************************************************
@file: shadow.h
@brief settings for shadows
****************************************************************************************************
***************************************************************************************************/
#ifndef _SHADOW_H_
#define _SHADOW_H_
#include "globals.h"

const int Z_SELECT_SIZE = 64;             //selection size of z-buffer (to compute average depth)
//near and far shadow planes
const float SHADOW_NEAR = 0.1f;
const float SHADOW_FAR = 5000.0f;

//FIXME: ovlivnuje vliv hodnoty gradientu na posunuti
const float POLY_BIAS = 0.5f;

const int MAX_CASCADES = 5;             //maximum shadow cascade count
const int F_POINTS = 4;                 //near/far point count

//bias matrix
const glm::mat4 biasMatrix = glm::mat4( 0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f);

#endif
