#pragma once
#include "cl/cl.h"
#include <vector>
#include <memory.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
// Structures
// 

struct ocl_args_d_t
{
	ocl_args_d_t();
	~ocl_args_d_t();

	// Regular OpenCL objects:
	cl_context       context;           // hold the context handler
	cl_device_id     device;            // hold the selected device handler
	cl_command_queue commandQueue;      // hold the commands-queue handler
	cl_program       program;           // hold the program handler
	cl_kernel        kernel;            // hold the kernel handler
	float            platformVersion;   // hold the OpenCL platform version (default 1.2)
	float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
	float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)

	// Objects that are specific for algorithm implemented in this sample
	cl_mem           srcA;              // hold first source buffer
	cl_mem           srcB;              // hold second source buffer
	cl_mem           dstMem;            // hold destination buffer
};

//////////////////////////////////////////////////////////////////////////
// Functions
// 

/*const char* TranslateOpenCLError(cl_int errorCode);
bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform);
cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType);
int GetPlatformAndDeviceVersion(cl_platform_id platformId, ocl_args_d_t *ocl);
int SetupOpenCL(ocl_args_d_t *ocl, cl_device_type deviceType);
int CreateAndBuildProgram(ocl_args_d_t *ocl);
int CreateBufferArguments(ocl_args_d_t *ocl, cl_int* inputA, cl_int* inputB, cl_int* outputC, cl_uint arrayWidth, cl_uint arrayHeight);
cl_uint SetKernelArguments(ocl_args_d_t *ocl);*/
void gpuProcess(ocl_args_d_t* oclA, unsigned char* data, unsigned long dataLength, unsigned int pX, unsigned int pY, int iGPU);
unsigned int gpuCount();
