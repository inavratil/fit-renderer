/**
****************************************************************************************************
****************************************************************************************************
@file: compute.cpp
@brief implementation of basic interaction with OpenCL interface
Code adapted from: ATI Stream SDK, http://developer.amd.com/gpu/ATIStreamSDK/Pages/default.aspx
****************************************************************************************************
***************************************************************************************************/

#include "compute.h"

/****************************************************************************************************
@brief Set some basic values
***************************************************************************************************/
TCompute::TCompute()
{
  maxWorkItemSizes = NULL;
  groupSize = 256;
  maxWorkGroupSize = 256;
  pos = NULL;
}


/****************************************************************************************************
@brief Load kernel source from file
@param source file with kernel source
***************************************************************************************************/
string TCompute::LoadSource(const char* source)
{
  ifstream fin(source);
  if(!fin)
  {
    string msg = "Cannot open kernel source ";
    msg += source;
    ShowMessage(msg.c_str(), false);
    return "null";
  }
  string data;
  char ch;
  while(fin.get(ch))
    data+=ch;

  return data;
}


/****************************************************************************************************
@brief Initialize OpenCL device, create context and command queue
@return success/fail of initialization
***************************************************************************************************/
bool TCompute::InitCL()
{
	//get platform
  cl_platform_id platform = NULL;
	status = clGetPlatformIDs(1, &platform, NULL);
	if(status != CL_SUCCESS)
	  return false;

	//find the GPU CL device
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	assert(device);

	//get some information about the returned device
	cl_char vendor_name[1024] = {0};
	cl_char device_name[1024] = {0};
	status = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor_name), vendor_name, NULL);
	status |= clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
	assert(status == CL_SUCCESS);
	printf("Connecting to %s %s...\n", vendor_name, device_name);

	//create a context to perform our calculation with the specified device
	context = clCreateContext(0, 1, &device, NULL, NULL, &status);
	assert(status == CL_SUCCESS);

	//command queue for the context
	commandQueue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);

  //some device specific Information
  status = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), (void*)&maxWorkGroupSize, NULL);
  status = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), (void*)&maxDimensions, NULL);
  maxWorkItemSizes = new size_t[maxDimensions];
  status = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * maxDimensions, (void*)maxWorkItemSizes, NULL);
  status = clGetDeviceInfo(device,  CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), (void *)&totalLocalMemory, NULL);
	assert(status == CL_SUCCESS);

	return true;
}


/****************************************************************************************************
@brief Load, compile and setup kernel object
@param source_file path to file with kernel source
@param func_name function name in kernel
@return success/fail of kernel creation
***************************************************************************************************/
bool TCompute::SetupKernel(const char* source_file, const char* func_name)
{
  //create a CL program using the kernel source
	string str_source = LoadSource(source_file);
	const char* source = str_source.c_str();
	size_t sourceSize[] = { str_source.length() };
	program = clCreateProgramWithSource(context,1, &source, sourceSize, &status);
	assert(status == CL_SUCCESS);

  //create a cl program executable for all the devices specified
  status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
  if(status != CL_SUCCESS)
  {
    if(status == CL_BUILD_PROGRAM_FAILURE)
    {
			ShowMessage("Build program failed!",false);
      cl_int logStatus;
      char * buildLog = NULL;
      size_t buildLogSize = 0;
      logStatus = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, &buildLogSize);
      buildLog = new char[buildLogSize];
      assert(buildLog != NULL);
      memset(buildLog, 0, buildLogSize);

      logStatus = clGetProgramBuildInfo (program, device, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
			assert(logStatus == CL_SUCCESS);

      cout<<buildLog<<endl;
      delete [] buildLog;
    }
		return false;
  }

  //get a kernel object handle for a kernel with the given name
  kernel = clCreateKernel(program, func_name, &status);
	assert(status == CL_SUCCESS);

  //get optimal work group memory size from device
	status = clGetKernelWorkGroupInfo(kernel, device,  CL_KERNEL_LOCAL_MEM_SIZE, sizeof(cl_ulong), &usedLocalMemory, NULL);
	assert(status == CL_SUCCESS);
  if(usedLocalMemory > totalLocalMemory)
  {
      ShowMessage("Unsupported: Insufficient local memory on device.",false);
      return false;
  }

	groupSize = 256;
	//check if group size isn't greater than supported
  if(groupSize > maxWorkItemSizes[0] || groupSize > maxWorkGroupSize)
  {
		//then get optimal work group size
		status = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &groupSize, 0);
		assert(status == CL_SUCCESS);
  }
	cout<<"Work group size: "<<groupSize<<endl;

	return SetupKernelArgs();
}

/****************************************************************************************************
@brief Destroy OpenCL context and program objects
@return success/fail of data release
***************************************************************************************************/
bool TCompute::Destroy()
{
  //releases OpenCL resources (Context, Memory etc.)
  cl_int status;

  status = clReleaseKernel(kernel);
  status |= clReleaseProgram(program);
  status |= clReleaseCommandQueue(commandQueue);
  status |= clReleaseContext(context);

  if(maxWorkItemSizes)
    delete [] maxWorkItemSizes;

	if(status == CL_SUCCESS)
		return true;
	else
		return false;
}

