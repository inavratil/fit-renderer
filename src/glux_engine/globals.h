/**
****************************************************************************************************
****************************************************************************************************
@file globals.h
@brief common OpenGL and system includes, extern variables and enum types
****************************************************************************************************
***************************************************************************************************/
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//#include <vld.h>


///Windows libraries
#if defined (__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
  #define _WIN_
  #include <windows.h>
#else
///Linux libraries
    #define _LINUX_
	#include <sys/time.h>
#endif

//SDL
#ifndef __WXMSW__
  #include <SDL/SDL.h>
#endif

///OpenGL/GLEW libraries
#include <GL/glew.h>

///OpenCL library
#include <CL/cl.h>
#include <CL/cl_gl.h>

//GLM
#include <glm/glm.hpp>
//#include <glm/gtx/comparison.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//AntTweakBar
#include "AntTweakBar.h"

///C/C++ system libraries
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <malloc.h>
#include <memory.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <algorithm>

///verbose mode
//#define VERBOSE

#define SHADOW_MULTIRES

using namespace std;
///Error code
const int ERR = -1;					

///Axis of rotation/scale
enum Axes {A_X, A_Y, A_Z};  

///Light models
enum LightModels{GOURAUD,PHONG,NONE,SCREEN_SPACE};
///Light components
enum LightComponents{AMBIENT,DIFFUSE,SPECULAR};
///light/shadow types
enum LightTypes{SPOT, OMNI};
///dual-paraboloid shadow parameters
//enum DPShadowParams{CUT, AUTOROT};
enum DPShadowMethods{DPSM, IPSM, CUT, WARP_DPSM};


///Shader types
enum ShaderTypes{VERTEX, TESS_CONTROL, TESS_EVAL, GEOMETRY, FRAGMENT};

///Texture types
enum TextureTypes{BASE,ENV,BUMP,PARALLAX,DISPLACE,CUBEMAP,CUBEMAP_ENV, ALPHA, SHADOW, SHADOW_OMNI, 
                  RENDER_TEXTURE, RENDER_TEXTURE_MULTISAMPLE};
///Texture mixing mods
enum TextureMods{ADD,MODULATE,DECAL,BLEND,REPLACE};

///Font types
enum font_type{NORMAL,TIMES,HELVETICA};
///Font sizes
enum font_size{SMALL,MEDIUM,LARGE};

///Scene draw mode
enum DrawMode{DRAW_ALL, DRAW_TRANSPARENT, DRAW_OPAQUE, DRAW_ALPHA};

//Uniform buffer indices
enum UniformIndices{UNIFORM_MATRICES, UNIFORM_LIGHTS};

///Camera types
enum CamTypes{FPS, ORBIT};

///FBO creation mode
enum FBOmode { NO_DEPTH, DEPTH_ONLY, DEPTH_AND_STENCIL };

#define PI 3.1415926535898f

/**
****************************************************************************************************
@brief Show message box (or output to console on Linux)
@param msg message to be shown
@param info if true, show information box. Else show error box
****************************************************************************************************/
inline void ShowMessage(const char *msg,bool info=true)
{
  if(!info)
  {
  #ifdef _WIN_
    MessageBox(NULL,msg,"ERROR",MB_ICONERROR);
  #else
    cerr<<"ERROR: "<<msg<<endl;
  #endif
  }
  else
  {
  #ifdef _WIN_
    MessageBox(NULL,msg,"Info",MB_ICONINFORMATION);
  #else
    cout<<msg<<endl;
  #endif
  }
}

/**
****************************************************************************************************
@brief Convert number into C++ string
@param t number to be converted
@param precision floating point precision
@return string with number
****************************************************************************************************/
template <class T> inline string num2str(const T& t, int precision = -1)
{
	stringstream ss;
    if(precision > 0)
    {
	    ss.setf(ios::fixed);
        ss.precision(precision);
    }
    
    ss << t;
	return ss.str();
}

/**
****************************************************************************************************
@brief Print content of GLM matrix
***************************************************************************************************/
inline void printMatrix( glm::mat4 m, const char* name = NULL )
{
    if( name )
        cout << name << endl;
    cout << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << endl;
    cout << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << endl;
    cout << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << endl;
    cout << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << endl;
    cout << "-----------------------" << endl;
}

/**
****************************************************************************************************
@brief Print content of GLM vector
***************************************************************************************************/
inline void printVec( glm::vec4 v, const char* name = NULL )
{
    if( name )
        cout << name << endl;
    cout << v.x << ", " << v.y << ", " << v.z << ", " << v.w << endl;
}


    const glm::mat4 cam_matrix = glm::mat4(
    // katedrala exp 4
    0.156434f, 0.0860827f, -0.98393f, 0.0f,
    0, 0.996195f, 0.0871557f, 0.0f,
    0.987688f, -0.0136342f, 0.155839f, 0.0f,
    -1.57116f, -63.7559f, -210.799f, 1.0f
    
    // scena s autem exp 5
    //-0.981627f, 0.0133102f, 0.190344f, 0.0f,
    //0.0f, 0.997564f, -0.0697565f, 0.0f,
    //-0.190809f, -0.0684748f, -0.979236f, 0.0f,
    //-52.4424f, -21.0609f, -200.104f, 1.0f

    // scene 5 - dbg_plane     
    //0.0f, 0.0f, -1.0f, 0.0f,
    //0.0f, 1.0f, 0.0f, 0.0f,
    //1.0f, 0.0f, 0.0f, 0.0f,
    //5.0f, -5.0f, -17.1f, 1.0f

    // scene 5 - dbg_plane - kamera s perspektivni projekci, v pozici svetla
    //-1.0f, 0.0f, 0.0f, 0.0f,
    //0.0f, 1.0f, 0.0f, 0.0f,
    //1.0f, 0.0f, -1.0f, 0.0f,
    //0.0f, -5.0f, -12.071f, 1.0f
        );

#endif
