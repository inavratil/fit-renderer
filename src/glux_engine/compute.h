/**
****************************************************************************************************
****************************************************************************************************
@file: compute.h
@brief class containing basic interaction with OpenCL interface
Code adapted from: ATI Stream SDK, http://developer.amd.com/gpu/ATIStreamSDK/Pages/default.aspx
****************************************************************************************************
***************************************************************************************************/

#ifndef _COMPUTE_H_
#define _COMPUTE_H_
#include "globals.h"
#include <assert.h>

/**
@class TCompute
@brief contains variables and functions to basic interact with OpenCL interface
***************************************************************************************************/
class TCompute
{
	protected:
		cl_device_id device;								//CL device ID
		cl_context context;                 //CL context
		cl_command_queue commandQueue;      //CL command queue
		cl_program program;                 //CL program
		cl_kernel kernel;                   //CL kernel
		cl_int status;											//error status

		size_t groupSize;                   //work-group size
		size_t* maxWorkItemSizes;           //max work-items sizes in each dimensions
		size_t maxWorkGroupSize;            //nax allowed work-items in a group
		cl_uint maxDimensions;              //max group dimensions allowed
		cl_ulong totalLocalMemory;          //max local memory allowed
		cl_ulong usedLocalMemory;           //used local memory

		cl_float dT;												//timestep

	public:
		cl_float* pos;											//current position

		TCompute();
		///@brief destroy OpenCL context
		virtual ~TCompute()
		{ Destroy(); }
		bool InitCL();
		string LoadSource(const char* source);
		bool SetupKernel(const char* source_file, const char* func_name);

		virtual bool Destroy();

		///these functions must be defined in derived classes
		virtual bool Init() 
		{ return true; }
		virtual bool SetupKernelArgs() 
		{ return true; }
		virtual bool Run() 
		{ return true; }
};



#endif
