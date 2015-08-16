//////////////////////////////////////////////////////////////////////////
// Needed Headers for this file
// 
#include "CL/cl.h"

//////////////////////////////////////////////////////////////////////////
// Classes
// 
class OpenCLHandle
{
public:
	OpenCLHandle();
	~OpenCLHandle();

	// Regular OpenCL objects:
	cl_context       context;           // hold the context handler
	cl_device_id     device;            // hold the selected device handler
	cl_command_queue commandQueue;      // hold the commands-queue handler
	cl_program       program;           // hold the program handler
	cl_kernel        kernel;            // hold the kernel handler
	float            platformVersion;   // hold the OpenCL platform version (default 1.2)
	float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
	float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)

	// Memory used by OpenCL Runtime
	cl_mem inImage; // Image Data
	cl_mem trPoseImage; // image as a float4 RGBL Split
};