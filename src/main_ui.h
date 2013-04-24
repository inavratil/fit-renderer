//MAIN APPLICATION VARIABLES, SETTINGS AND TWEAK BARS

#include "glux_engine/engine.h"
#include "experiments.h"

using namespace std;

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