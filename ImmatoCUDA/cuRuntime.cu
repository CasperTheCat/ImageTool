
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuRuntime.cuh"
#include <iostream>
#include <stdio.h>
/*#include "helper_math.h"
#include "helper_image.h"*/
/*#include "cuda_fp16.h"
#include "cudaGL.h"
#include "cudaD3D11.h"
#include "cuda_d3d11_interop.h"*/
/*#include "math_functions.h"*/

#define LUMA float3{float(0.2126), float(0.7152), float(0.0722)}

// Define LUMA
//float3 LUMA{ float(0.2126), float(0.7152), float(0.0722) };


__device__ inline float dot(float3 iX, float3 iY)
{
	return (iX.x * iY.x) + (iX.y * iY.y) + (iX.z * iY.z);
}

__device__ inline float dot(float iX, float3 iY)
{
	return (iX * iY.x) + (iX * iY.y) + (iX * iY.z);
}

__device__ inline float dot(float4 iX, float3 iY)
{
	return (iX.x * iY.x) + (iX.y * iY.y) + (iX.z * iY.z);
}



// REM cudaFree
#pragma region Define Operators
__device__
float3 operator- (float3 f1, float f2)
{
	return float3{ f1.x - f2, f1.y - f2, f1.z - f2 };
}

__device__
float3 operator+ (float3 f1, float f2)
{
	return float3{ f1.x + f2, f1.y + f2, f1.z + f2 };
}

__device__
float3 operator/ (float3 f1, float f2)
{
	return float3{ f1.x / f2, f1.y / f2, f1.z / f2 };
}

__device__
float3 operator* (float3 f1, float f2)
{
	return float3{ f1.x * f2, f1.y * f2, f1.z * f2 };
}


#pragma endregion
__host__ __device__
int Modulus(int a, int b)
{
	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

template <typename T>
__host__ __device__
inline T lerp(T v0, T v1, T t)
{
	return fma(t, v1, fma(-t, v0, v0));
}

__device__ inline float gpuClamp(float in_t, int high_i, int low_i)
{
	return (in_t < low_i) ? low_i : (in_t > high_i) ? high_i : in_t;
}

__device__ inline float gpuClamp(float in_t)
{
	return (in_t < 0.f) ? 0.f : (in_t > 1.f) ? 1.f : in_t;
}

__device__ inline float CTF(unsigned char in)
{
	return (float(in) / float(255));
}

__device__ inline unsigned char FTC(float in)
{
	return unsigned char(gpuClamp(in) * 255);
}

// PreProcessor, does a transpose op
__global__ void kPreProcess(unsigned char* data, float3* out, uint64_t len)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t dataLocation = gIdx * 4;
	if (dataLocation >= len) return;

	out[gIdx].x = CTF(data[dataLocation]);
	out[gIdx].y = CTF(data[dataLocation + 1]);
	out[gIdx].z = CTF(data[dataLocation + 2]);

}

// PostProcessor, does a reverse transpose op
__global__ void kPostProcess(unsigned char* data, float3* in, uint64_t len)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t dataLocation = gIdx * 4;
	if (dataLocation >= len) return;

	data[dataLocation] = FTC(in[gIdx].x);
	data[dataLocation + 1] = FTC(in[gIdx].y);
	data[dataLocation + 2] = FTC(in[gIdx].z);
	//data[dataLocation + 3] = FTC(1.f);
	// ignore alpha. We left it in the array
}

__device__ float lumaSharp(float3* in, uint64_t len, uint64_t idx, uint32_t pX, uint32_t pY, int fW)
{
	float difUp = dot(in[Modulus(idx - pX * fW, pX * pY)], LUMA);
	float difDown = dot(in[Modulus(idx + pX * fW, pX * pY)], LUMA);
	float difLeft = dot(in[Modulus(idx - 1 * fW, pX * pY)], LUMA);
	float difRight = dot(in[Modulus(idx + 1 * fW, pX * pY)], LUMA);

	// Non Immed
	float difUpRight = dot(in[Modulus((idx - pX * fW) + 1 * fW, pX * pY)], LUMA);
	float difDownLeft = dot(in[Modulus((idx + pX * fW) - 1 * fW, pX * pY)], LUMA);
	float difUpLeft = dot(in[Modulus((idx - pX * fW) - 1 * fW, pX * pY)], LUMA);
	float difDownRight = dot(in[Modulus((idx + pX * fW) + 1 * fW, pX * pY)], LUMA);


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

	/*return (difUp * 0.125f
	+ difDown * 0.125f
	+ difLeft *  0.125f
	+ difRight *  0.125f
	+ difUpRight *  0.125f
	+ difUpLeft *  0.125f
	+ difDownLeft *  0.125f
	+ difDownRight *  0.125f);*/
}


// Main Processor step
__global__ void kProcessStepOne(float3* in, float3* cOut, float* lOut, uint64_t len, uint32_t pX, uint32_t pY)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t dataLocation = gIdx * 4;
	if (gIdx >= pY*pX) { return; }

	//float3 temp = in[gIdx];
	// Strip Luma!
	float pLuma = dot(in[gIdx], LUMA);
	float3 pChro = in[gIdx] - pLuma;



	// Sharpen - Get Blur
	float blur = lumaSharp(in, len, gIdx, pX, pY,1);


	// Sharpen - Sub Blur from Luma
	float sharp = pLuma - blur;




	// Bloom?

	// Edge Detect


	// Contrast
	pChro = ((pChro - 0.5f) * 1.1f) + 0.5f;

	// % Brightness
	float vLen = dot(1.0, pChro) / 3;
	pChro = pChro * (1.25f + vLen);
	//pChro = pChro * 1.5f;

	cOut[gIdx] = pChro;
	lOut[gIdx] = pLuma + dot(sharp, LUMA);

	/*__syncthreads();
	in[gIdx] = pChro + pLuma + dot(sharp, LUMA * 2);*/
	//in[gIdx] = { blur,blur,blur };
}

// Stitch Buffers
__global__ void kProcessStepTwo(float3* out, float3* cIn, float* lIn, uint64_t len, uint32_t pX, uint32_t pY)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t dataLocation = gIdx * 4;
	if (gIdx >= pY*pX) { return; }

	// Stitch data
	out[gIdx] = cIn[gIdx] + lIn[gIdx];
	//in[gIdx] =  + pLuma + dot(sharp, LUMA * 2);
	//in[gIdx] = { blur,blur,blur };
}


bool _GPUSelected =false;
void gpuSelector(unsigned char* data, uint64_t len, uint32_t pX, uint32_t pY)
{
	// Get Device
	int deviceCount;
	cudaGetDeviceCount(&deviceCount);
	if (!_GPUSelected)
	{
		if (deviceCount == 0)
		{
			std::cout << "CUDA Capable GPU was not detected" << std::endl;
			return;
		}
		else
		{
			std::cout << deviceCount << " CUDA capable GPUs were detected" << std::endl;
			_GPUSelected = true;
		}
	}

	if (deviceCount > 1) 
	{
		cudaSetDevice(0);
		std::cout << "Using Device 1" << std::endl;
	}
	else
	{
		cudaSetDevice(0);
		std::cout << "Using Device 0" << std::endl;
	}

	// Threading
	uint32_t threadCount = 1024;
	uint32_t blockCount = (pY * pX) / threadCount + 1;

	// Setup the transpose!
	float3* imageData;
	cudaMalloc((void**)&imageData, sizeof(float3) * pX * pY);
	float3* chromaData;
	cudaMalloc((void**)&chromaData, sizeof(float3) * pX * pY);
	float* lumaData;
	cudaMalloc((void**)&lumaData, sizeof(float) * pX * pY);

	std::cout << "cMalloc: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run Transpose
	kPreProcess << < blockCount, threadCount >> > (data, imageData, len);
	cudaDeviceSynchronize();
	std::cout << "PreProcess: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run Process
	kProcessStepOne << < blockCount, threadCount >> >(imageData, chromaData, lumaData, len, pX, pY);
	cudaDeviceSynchronize();
	std::cout << "Process: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Sync the GPU. We need step 1 to finish before step is scheduled
	cudaDeviceSynchronize();
	std::cout << "Synchronize: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run Stitch
	kProcessStepTwo << < blockCount, threadCount >> >(imageData, chromaData, lumaData, len, pX, pY);
	std::cout << "Stitch " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	cudaFree(chromaData);
	cudaFree(lumaData);
	std::cout << "Free: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// Run antiTranspose
	kPostProcess << < blockCount, threadCount >> > (data, imageData, len);


	// Sync
	cudaDeviceSynchronize();
	std::cout << "PostProcess: " << cudaGetErrorString(cudaGetLastError()) << std::endl;

	// free
	cudaFree(imageData);
}


void gpuProcess(unsigned char* data, uint64_t dataLength, uint32_t pX, uint32_t pY)
{
	// MEMCPY to GPU
	unsigned char* deviceData;
	cudaMalloc((void**)&deviceData, sizeof(unsigned char) * dataLength);


	cudaMemcpy(deviceData, data, dataLength, cudaMemcpyHostToDevice);


	gpuSelector(deviceData, dataLength, pX, pY);



	cudaMemcpy(data, deviceData, dataLength, cudaMemcpyDeviceToHost);

	cudaFree(deviceData);

	// Make sure GPU is in a good state!
	cudaDeviceReset();
}
