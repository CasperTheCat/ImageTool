#include "clDecl.h"

#define OPENCL_VERSION_1_2 1.2f

//////////////////////////////////////////////////////////////////////////
// Construct the Handle
// 
OpenCLHandle::OpenCLHandle()
{
	context = (NULL);
	device = (NULL);
	commandQueue = (NULL);
	program = (NULL);
	kernel = (NULL);
	platformVersion = (OPENCL_VERSION_1_2);
	deviceVersion = (OPENCL_VERSION_1_2);
	compilerVersion = (OPENCL_VERSION_1_2);
	inImage = (NULL);
	trPoseImage = (NULL);
}

//////////////////////////////////////////////////////////////////////////
// Destruct the used resources
// 
OpenCLHandle::~OpenCLHandle()
{
	if (kernel)	clReleaseKernel(kernel);
	if (program) clReleaseProgram(program);
	if (inImage) clReleaseMemObject(inImage);
	if (trPoseImage) clReleaseMemObject(trPoseImage);
	if (commandQueue) clReleaseCommandQueue(commandQueue);
	if (device) clReleaseDevice(device);
	if (context) clReleaseContext(context);
}