//-- Shader generator strings
//-- All variables have prefix SG (stands for Shader Generator)

//-- Notation:
//	Object Space:	Eye Space:			Normalized:
//	vertCoord		vertEyeSpace		norm_vertEyeSpace
//	normalCoord		normalEyeSpace		unit_normalEyeSpace
//	texCoord		---					---


#ifndef _SHADERGENERATOR_H_
#define _SHADERGENERATOR_H_

#include <string>

using namespace std;

//------------------------------------------------------------------------------
//-- Types & Keywords

const string SG_IN =			"in ";
const string SG_OUT =			"out ";
const string SG_UNIFORM =		"uniform ";
const string SG_VEC4 =			"vec4 ";
const string SG_SAMPLER2D =		"sampler2D ";

//------------------------------------------------------------------------------
//-- Variables

//-- Input
const string SG_MAT_PROJ =		"in_ProjectionMatrix ";		// projekcni matice

//-- Main
const string SG_V4_VERTCOORD =	"vertCoord ";				// souradnice vrcholu v objektovem prostoru

//------------------------------------------------------------------------------
//-- Miscellaneous

const string SG_ASSIGN =		" = ";

const string SG_NEWLINE =		"\n";
const string SG_SEMI =			";";
const string SG_SEMINL =		";\n";
const string SG_TAB =			"\t";

//------------------------------------------------------------------------------
//-- Functions

string LoadFunc(char* func, char* type = ".frag" );

#endif
