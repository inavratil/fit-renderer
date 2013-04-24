//MAIN APPLICATION VARIABLES, SETTINGS AND TWEAK BARS

#include "glux_engine/engine.h"
#include "experiments.h"

using namespace std;

//FPS
unsigned cycle = 0;                     //drawn cycles in one second 
unsigned fps;

//scene settings
TScene *s;          
string msg;
bool wire = false;                    
int mem_use = 0;
bool draw_ui = true;

//scenes
int scene = 1;
int ex = -1;

//SHADOW CONTROLS

#define USE_DP

//paraboloid rotation
bool move_parab = false;
glm::vec3 parab_rot(0.0, 0.0, 0.0);

//paraboloid shadow map settings
bool parabola_cut = false;
bool dpshadow_tess = false;
bool use_pcf = false;
int dpshadow_method = WARP_DPSM;

Experiment exper;
bool isExperiment = false;

int SHADOW_RES = 1024;

//multires settings
int mip_level = 0;
float normal_threshold = 1.0;
float depth_threshold = 15.0;


#if 0

///////////////////////////////////////////////////////////////////////////////////////////////////
// TWEAK BAR

//CALLBACK FUNCTIONS

//Save/load camera
void TW_CALL twSaveCamera(void *clientData){
    s->SaveCamera();
}
void TW_CALL twLoadCamera(void *clientData){
    s->LoadCamera();
}

//Reset paraboloid
void TW_CALL twResetParab(void *clientData){
    parab_rot.x = 0.0;
    parab_rot.y = 0.0; 
    s->RotateParaboloid(parab_rot);
    cut_angle = glm::vec2( 0.0 );
    s->DPSetCutAngle(cut_angle);
}


/**
******************************************************************************
* Init tweak bar with gadgets and info
******************************************************************************/
TwBar *ui;
void InitTweakBar()
{

    //camera
    TwEnumVal e_cam_type[] = { {FPS, "FPS"}, {ORBIT, "Orbit"}};
    TwType e_cam_types = TwDefineEnum("cam_types", e_cam_type, 2);
    TwAddVarRW(ui, "cam_mode", e_cam_types, &cam_type, "label='Type' key=c group='Camera'");
    //camera save/load
    TwAddButton(ui, "cam_save", twSaveCamera, NULL, "label='Save' group='Camera' key=F5"); 
    TwAddButton(ui, "cam_load", twLoadCamera, NULL, "label='Load' group='Camera' key=F9"); 
}

/**
******************************************************************************
* Update values bound with tweak bar
******************************************************************************/
void UpdateTweakBar()
{
    s->SetCamType(cam_type);
	
}

#endif