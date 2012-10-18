/**********************************************************************************
 * Copyright (c) 2008-2009 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 **********************************************************************************/

#ifndef __OPENCL_CL_D3D10_H
#define __OPENCL_CL_D3D10_H

#include "cl_platform.h"
#include "cl.h"

#ifdef __cplusplus
extern "C" {
#endif

// cl_context_properties
#define CL_CONTEXT_D3D10_DEVICE 0x1086

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10BufferKHR(
    cl_context     /* context */,
    cl_mem_flags   /* flags */,
    ID3D10Buffer * /* buffer */,
    cl_int *       /* errcode_ret */);


extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10Texture1DKHR(
    cl_context        /* context */,
    cl_mem_flags      /* flags */,
    ID3D10Texture1D * /* resource */,
    UINT              /* subresource */,
    cl_int *          /* errcode_ret */);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10Texture2DKHR(
    cl_context        /* context */,
    cl_mem_flags      /* flags */,
    ID3D10Texture2D * /* resource */,
    UINT              /* subresource */,
    cl_int *          /* errcode_ret */);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10Texture3DKHR(
    cl_context        /* context */,
    cl_mem_flags      /* flags */,
    ID3D10Texture3D * /* resource */,
    UINT              /* subresource */,
    cl_int *          /* errcode_ret */);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireD3D10ObjectsKHR(
    cl_command_queue /* command_queue */,
    cl_uint          /* num_objects */,
    const cl_mem *   /* mem_objects */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* event */);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseD3D10ObjectsKHR(
    cl_command_queue /* command_queue */,
    cl_uint          /* num_objects */,
    const cl_mem *   /* mem_objects */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* event */);

#ifdef __cplusplus
}
#endif

#endif  // __OPENCL_CL_D3D10_H

