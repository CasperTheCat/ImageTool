//#include "clRuntime.h"
//#include <iostream>
//#include <stdio.h>
//#include "clDefinitions.h"


// OPTIONS
// These are options, they will be made later

// Darkness
#define darkPivot -0.1f // DEF 0.1f

// uSharp
#define uSharpStrength 1.f
#define uSharpWidth 1

// Contrast
#define contraStrength 1.1f
#define contraPivot 0.5f

// Saturation
#define satStrength 1.5f

// Experimental
#define r_experimental 0


// END OPTIONS

#define LUMA cl_float3{cl_cl_float(0.2126), cl_cl_float(0.7152), cl_cl_float(0.0722)}

// Define LUMA
//cl_float3 LUMA{ cl_float(0.2126), cl_float(0.7152), cl_float(0.0722) };


/*inline cl_float dot(cl_float3 iX, cl_float3 iY)
{
return (iX.x * iY.x) + (iX.y * iY.y) + (iX.z * iY.z);
}

inline cl_float dot(cl_float iX, cl_float3 iY)
{
return (iX * iY.x) + (iX * iY.y) + (iX * iY.z);
}

inline cl_float dot(cl_float4 iX, cl_float3 iY)
{
return (iX.x * iY.x) + (iX.y * iY.y) + (iX.z * iY.z);
}


#pragma region Define Operators
cl_float3 operator- (cl_float3 f1, cl_float f2)
{
return cl_float3{ f1.x - f2, f1.y - f2, f1.z - f2 };
}

cl_float3 operator- (cl_float3 f1, cl_float3 f2)
{
return cl_float3{ f1.x - f2.x, f1.y - f2.y, f1.z - f2.z };
}

cl_float3 operator+ (cl_float3 f1, cl_float f2)
{
return cl_float3{ f1.x + f2, f1.y + f2, f1.z + f2 };
}


cl_float3 operator+ (cl_float3 f1, cl_float3 f2)
{
return cl_float3{ f1.x + f2.x, f1.y + f2.y, f1.z + f2.z };
}


cl_float3 operator/ (cl_float3 f1, cl_float f2)
{
return cl_float3{ f1.x / f2, f1.y / f2, f1.z / f2 };
}


cl_float3 operator/ (cl_float f1, cl_float3 f2)
{
return cl_float3{ f1 / f2.x, f1 / f2.y, f1 / f2.z };
}


cl_float3 operator* (cl_float3 f1, cl_float f2)
{
return cl_float3{ f1.x * f2, f1.y * f2, f1.z * f2 };
}


cl_float3 operator* (cl_float3 f1, cl_float3 f2)
{
return cl_float3{ f1.x * f2.x, f1.y * f2.y, f1.z * f2.z };
}*/


#pragma endregion
/*
#pragma region Helpers
int Modulus(int a, int b)
{
	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

template <typename T>
__host__
inline T lerp(T v0, T v1, T t)
{
	return fma(t, v1, fma(-t, v0, v0));
}

inline cl_float3 lerp(cl_float3 fX, cl_float3 fY, cl_float fZ)
{
	return fX*(1 - fZ) + fY*fZ;
}

inline cl_float gpuClamp(cl_float in_t, int high_i, int low_i)
{
	return (in_t < low_i) ? low_i : (in_t > high_i) ? high_i : in_t;
}

inline cl_float gpuClamp(cl_float in_t)
{
	return (in_t < 0.f) ? 0.f : (in_t > 1.f) ? 1.f : in_t;
}

inline cl_float CTF(unsigned char in)
{
	return (cl_float(in) / cl_float(255));
}

inline unsigned char FTC(cl_float in)
{
	return unsigned char(gpuClamp(in) * 255);
}

#pragma endregion

// PreProcessor, does a transpose op
__kernel void kPreProcess(__global unsigned char* data, __global cl_float3* out, __global unsigned long len)
{
	// OCL
	unsigned long gIdx = get_global_id(1) * get_global_size(0) + get_global_id(0);
	unsigned long dataLocation = gIdx * 4;
	if (dataLocation >= len) return;

	out[gIdx].x = CTF(data[dataLocation]);
	out[gIdx].y = CTF(data[dataLocation + 1]);
	out[gIdx].z = CTF(data[dataLocation + 2]);
}




// PostProcessor, does a reverse transpose op
__kernel void kPostProcess(__global unsigned char* data, __global cl_float3* in, __global unsigned long len)
{
	unsigned long gIdx = get_global_id(1) * get_global_size(0) + get_global_id(0);
	unsigned long dataLocation = gIdx * 4;
	if (dataLocation >= len) return;

	data[dataLocation] = FTC(in[gIdx].x);
	data[dataLocation + 1] = FTC(in[gIdx].y);
	data[dataLocation + 2] = FTC(in[gIdx].z);
	//data[dataLocation + 3] = FTC(1.f);
	// Skip alpha. We left it in the array
}


// Bloom Effect
// Experimental Bloom
//
cl_float3 r_bloom(cl_float3* aColor, unsigned long idx)
{
	// Not Yet Implemented
	return aColor[idx] * aColor[idx];
}


// Device lumaSharpen
// Sharpen the image with a uSharp Mask
//
cl_float lumaSharp(cl_float3* in, unsigned long len, unsigned long idx, unsigned int pX, unsigned int pY, int fW)
{
	cl_float difUp = dot(in[Modulus(idx - pX * fW, pX * pY)], LUMA);
	cl_float difDown = dot(in[Modulus(idx + pX * fW, pX * pY)], LUMA);
	cl_float difLeft = dot(in[Modulus(idx - 1 * fW, pX * pY)], LUMA);
	cl_float difRight = dot(in[Modulus(idx + 1 * fW, pX * pY)], LUMA);

	// Non Immed
	cl_float difUpRight = dot(in[Modulus((idx - pX * fW) + 1 * fW, pX * pY)], LUMA);
	cl_float difDownLeft = dot(in[Modulus((idx + pX * fW) - 1 * fW, pX * pY)], LUMA);
	cl_float difUpLeft = dot(in[Modulus((idx - pX * fW) - 1 * fW, pX * pY)], LUMA);
	cl_float difDownRight = dot(in[Modulus((idx + pX * fW) + 1 * fW, pX * pY)], LUMA);


	//__syncthreads();
	return (dot(in[idx], LUMA) * 0.12f
		+ difUp * 0.11f
		+ difDown * 0.11f
		+ difLeft * 0.11f
		+ difRight * 0.11f
		+ difUpRight * 0.11f
		+ difUpLeft * 0.11f
		+ difDownLeft * 0.11f
		+ difDownRight * 0.11f);
}



// Main Processor step
__kernel void kProcessStepOne(cl_float3* in, cl_float3* cOut, cl_float* lOut, cl_uint len, cl_uint pX, cl_uint pY)
{
	unsigned long gIdx = get_global_id(1) * get_global_size(0) + get_global_id(0);
	if (gIdx >= pY*pX) { return; }

	//cl_float3 temp = in[gIdx];
	// Strip Luma!
	cl_float pLuma = dot(in[gIdx], LUMA);
	cl_float3 pChro = in[gIdx] - pLuma;


	// pDarkness. Pivot / Color. 
	// Add in Central Shift
	// Mid Shift = 2Pivot, from Pivot / 0.5f 
	if (darkPivot > 0.f)
	{
		cl_float pDark = (darkPivot / pLuma) - (darkPivot * 2);
		pLuma -= pDark;
	}

	// Sharpen
	// This effect is similar to the Adobe UnSharp Mask
	cl_float sharpMask = 0.f; // Default to Zero
	if (uSharpWidth > 0.f && uSharpStrength > 0.f)
	{
		// Sharpen - Get Blur
		cl_float blur = lumaSharp(in, len, gIdx, pX, pY, uSharpWidth);
		// Sharpen - Sub Blur from Luma
		sharpMask = dot(pLuma - blur, LUMA * uSharpStrength);
	}




	// Bloom?
#if r_experimental
	pChro = r_bloom(in, gIdx);
#endif


	// Aberration
	//cl_float vLen = dot(1.0, pChro) / 3;
	//pChro = pChro * (1.25f + vLen);
	// or
	//pChro = pChro * ( 1.f + (dot(1.0, pChro) / 3));


	cOut[gIdx] = pChro;
	lOut[gIdx] = pLuma + sharpMask;
}

// Stitch Buffers
__kernel void kProcessStepTwo(cl_float3* out, cl_float3* cIn, cl_float* lIn, unsigned long len, unsigned int pX, unsigned int pY)
{
	unsigned long gIdx = get_global_id(1) * get_global_size(0) + get_global_id(0);
	if (gIdx >= pY*pX) { return; }

	cl_float3 pChro = cIn[gIdx];
	cl_float pLuma = lIn[gIdx];
	cl_float3 fColor;
	// Stitch data
	//out[gIdx] = cIn[gIdx] + lIn[gIdx];

	// Saturate and Stitch
	fColor = lerp(cl_float3{ pLuma, pLuma, pLuma }, pChro + pLuma, satStrength);


	// Contrast
	out[gIdx] = ((fColor - contraPivot) * contraStrength) + contraPivot;

}


bool _GPUSelected = false;
void clSelector(cl_mem data, unsigned long len, unsigned int pX, unsigned int pY, int fGPU)
{
	size_t globalWorkSize[2] = {pX,pY};

	// Data for runtime
	//cl_cl_float3* imageData = (cl_cl_float3*)_aligned_malloc(sizeof(cl_cl_float3) * pX * pY, 4096);
	cl_float3* chromaData = (cl_float3*)_aligned_malloc(sizeof(cl_float3) * pX * pY, 4096);
	cl_float* lumaData = (cl_float*)_aligned_malloc(sizeof(cl_float) * pX * pY, 4096);

	

	// Setup the transpose!
	cl_float3* imageData;
	cudaMalloc((void**)&imageData, sizeof(cl_float3) * pX * pY);
	cl_float3* chromaData;
	cudaMalloc((void**)&chromaData, sizeof(cl_float3) * pX * pY);
	cl_float* lumaData;
	cudaMalloc((void**)&lumaData, sizeof(cl_float) * pX * pY);

	//std::cout << "cMalloc: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run Transpose
	kPreProcess << < blockCount, threadCount >> > (data, imageData, len);
	cudaDeviceSynchronize();
	//std::cout << "PreProcess: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run Process
	kProcessStepOne << < blockCount, threadCount >> >(imageData, chromaData, lumaData, len, pX, pY);
	cudaDeviceSynchronize();
	//std::cout << "Process: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Sync the GPU. We need step 1 to finish before step is scheduled
	cudaDeviceSynchronize();
	//std::cout << "Synchronize: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run Stitch
	kProcessStepTwo << < blockCount, threadCount >> >(imageData, chromaData, lumaData, len, pX, pY);
	//std::cout << "Stitch " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	cudaFree(chromaData);
	cudaFree(lumaData);
	//std::cout << "Free: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run antiTranspose
	kPostProcess << < blockCount, threadCount >> > (data, imageData, len);


	// Sync
	cudaDeviceSynchronize();
	//std::cout << "PostProcess: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// free
	cudaFree(imageData);
}*/
/*
// OCL count
unsigned int gpuCount()
{
	cl_uint deviceCount;
	clGetPlatformIDs(0, NULL, &deviceCount);
	return deviceCount;
}

void gpuProcess(ocl_args_d_t* oclA, unsigned char* data, unsigned long dataLength, unsigned int pX, unsigned int pY, int iGPU)
{
	unsigned int oclC = gpuCount();

	// Make a vector of platforms
	std::vector<cl_platform_id> platforms(oclC);

	// get IDs
	clGetPlatformIDs(oclC, &platforms[0], NULL);

	size_t strLeng = 0;
	char* name;
	for (cl_uint i = 0; i < oclC;i++)
	{ 
		clGetPlatformInfo(platforms[i],CL_PLATFORM_NAME,0,NULL,&strLeng);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, strLeng, &name, NULL);
		std::cout << name << std::endl;
	}

	/*cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformId, 0 };
	oclA->context - clCreateContextFromType(contextProperties,deviceType, NULL,NULL,NULL);


	// Create a cl_mem buffer. This is a simple op, don't check failure state
	cl_mem deviceData = clCreateBuffer(oclA.context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, dataLength,data,NULL);
	
	// Fire off to the selector
	clSelector(deviceData, dataLength, pX, pY, iGPU);*/
//}




