
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuRuntime.cuh"
#include <iostream>
#include <stdio.h>


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


#pragma region Define Operators F4
__device__
float4 operator- (float4 f1, float f2)
{
	return float4{ f1.x - f2, f1.y - f2, f1.z - f2, f1.w };
}

__device__
float4 operator- (float4 f1, float4 f2)
{
	return float4{ f1.x - f2.x, f1.y - f2.y, f1.z - f2.z, f1.w };
}

__device__
float4 operator+ (float4 f1, float f2)
{
	return float4{ f1.x + f2, f1.y + f2, f1.z + f2, f1.w };
}

__device__
float4 operator+ (float4 f1, float4 f2)
{
	return float4{ f1.x + f2.x, f1.y + f2.y, f1.z + f2.z, f1.w };
}

__device__
float4 operator/ (float4 f1, float f2)
{
	return float4{ f1.x / f2, f1.y / f2, f1.z / f2, f1.w };
}

__device__
float4 operator/ (float f1, float4 f2)
{
	return float4{ f1 / f2.x, f1 / f2.y, f1 / f2.z, f2.w };
}

__device__
float4 operator* (float4 f1, float f2)
{
	return float4{ f1.x * f2, f1.y * f2, f1.z * f2, f1.w };
}

__device__
float4 operator* (float4 f1, float4 f2)
{
	return float4{ f1.x * f2.x, f1.y * f2.y, f1.z * f2.z, f1.w };
}


#pragma endregion

#pragma region Define Operators
__device__
float3 operator- (float3 f1, float f2)
{
	return float3{ f1.x - f2, f1.y - f2, f1.z - f2 };
}

__device__
float3 operator- (float3 f1, float3 f2)
{
	return float3{ f1.x - f2.x, f1.y - f2.y, f1.z - f2.z };
}

__device__
float3 operator+ (float3 f1, float f2)
{
	return float3{ f1.x + f2, f1.y + f2, f1.z + f2 };
}

__device__
float3 operator+ (float3 f1, float3 f2)
{
	return float3{ f1.x + f2.x, f1.y + f2.y, f1.z + f2.z };
}

__device__
float3 operator/ (float3 f1, float f2)
{
	return float3{ f1.x / f2, f1.y / f2, f1.z / f2 };
}

__device__
float3 operator/ (float f1, float3 f2)
{
	return float3{ f1 / f2.x, f1 / f2.y, f1 / f2.z };
}

__device__
float3 operator* (float3 f1, float f2)
{
	return float3{ f1.x * f2, f1.y * f2, f1.z * f2 };
}

__device__
float3 operator* (float3 f1, float3 f2)
{
	return float3{ f1.x * f2.x, f1.y * f2.y, f1.z * f2.z };
}


#pragma endregion

#pragma region Helpers
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

__device__ inline float3 lerp(float3 fX, float3 fY, float fZ)
{
	return fX*(1 - fZ) + fY*fZ;
}
__device__ inline float4 lerp(float4 fX, float4 fY, float fZ)
{
	return fX*(1 - fZ) + fY*fZ;
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

#pragma endregion


// PreProcessor, does a transpose op
__global__ void kPreProcess(unsigned char* data, float4* out, uint64_t len)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t dataLocation = gIdx * 4;
	if (dataLocation >= len) return;

	out[gIdx].x = CTF(data[dataLocation]);      // R
	out[gIdx].y = CTF(data[dataLocation + 1]);  // G
	out[gIdx].z = CTF(data[dataLocation + 2]);  // B
	out[gIdx].w = CTF(data[dataLocation + 3]);  // A
}




// PostProcessor, does a reverse transpose op
__global__ void kPostProcess(unsigned char* data, float4* in, uint64_t len)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	uint64_t dataLocation = gIdx * 4;
	if (dataLocation >= len) return;

	data[dataLocation] = FTC(in[gIdx].x);
	data[dataLocation + 1] = FTC(in[gIdx].y);
	data[dataLocation + 2] = FTC(in[gIdx].z);
	data[dataLocation + 3] = FTC(in[gIdx].w);
}


// Bloom Effect
// Experimental Bloom
//
__device__ float4 r_bloom(float4* aColor, uint64_t idx)
{
	// Not Yet Implemented
	return aColor[idx] * aColor[idx];
}


// Device lumaSharpen
// Sharpen the image with a uSharp Mask
//
__device__ float lumaSharp(float4* in, uint64_t idx, uint32_t pX, uint32_t pY, int fW)
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
}



// Main Processor step
__global__ void kProcess(float4* cIn, float4* cOut, uint32_t pX, uint32_t pY)
{
	uint64_t gIdx = blockIdx.x * blockDim.x + threadIdx.x;
	if (gIdx >= pY*pX) { return; }

	// Strip Luma!
	float pLuma = dot(cIn[gIdx], LUMA);
	float4 pChro = cIn[gIdx] - pLuma;


	// pDarkness. Pivot / Color. 
	// Add in Central Shift
	// Mid Shift = 2Pivot, from Pivot / 0.5f 
	if (darkPivot > 0.f)
	{
		float pDark = (darkPivot / pLuma) - (darkPivot * 2);
		pLuma -= pDark;
	}

	// Sharpen
	// This effect is similar to the Adobe UnSharp Mask
	float sharpMask = 0.f; // Default to Zero
	if (uSharpWidth > 0.f && uSharpStrength > 0.f)
	{
		// Sharpen - Get Blur
		float blur = lumaSharp(cIn, gIdx, pX, pY, uSharpWidth);
		// Sharpen - Sub Blur from Luma
		sharpMask = dot(pLuma - blur, LUMA * uSharpStrength);
	}

	// Bloom?
#if r_experimental
	pChro = r_bloom(in,gIdx);
#endif


	// Aberration
	//float vLen = dot(1.0, pChro) / 3;
	//pChro = pChro * (1.25f + vLen);
	// or
	//pChro = pChro * ( 1.f + (dot(1.0, pChro) / 3));

	// Saturate and restitch
	float4 cStitch = pChro + (pLuma + sharpMask);
	float4 sColor = lerp(float4{ (pLuma + sharpMask), (pLuma + sharpMask), (pLuma + sharpMask), pChro.w }, cStitch, satStrength);

	// Contrast and save to array
	cOut[gIdx] = ((sColor - contraPivot) * contraStrength) + contraPivot;
	//cOut[gIdx] = pChro +pLuma + sharpMask;
}

void gpuSelector(unsigned char* data, uint64_t len, uint32_t pX, uint32_t pY, int fGPU)
{
	// Useless call. Extremely low overhead so I'm remove and validate later
    cudaSetDevice(fGPU);
	
	// Threading values
	uint32_t threadCount = 1024;
	uint32_t blockCount = (pY * pX) / threadCount + 1;

	// Image data is Data from the CPU
	float4* inImageData;
	cudaMalloc((void**)&inImageData, sizeof(float4) * pX * pY);

	// Where finished data will go
	float4* outImageData;
	cudaMalloc((void**)&outImageData, sizeof(float4) * pX * pY);

	// Run Transpose - faster on GPU until I write the Intel SIMD stuff
	kPreProcess <<< blockCount, threadCount >>> (data, inImageData, len);
	
	// Sync the GPU. This is barrier
	cudaDeviceSynchronize();
	
	// Run Main
	kProcess <<< blockCount, threadCount >>>(inImageData, outImageData, pX, pY);
	
	// Sync the GPU. Another Barrier
	cudaDeviceSynchronize();

	// We are done with the initial data from the CPU
	cudaFree(inImageData);

	// Revert the formatting of the data
	kPostProcess <<< blockCount, threadCount >>> (data, outImageData, len);

	// Sync before final free
	cudaDeviceSynchronize();
	cudaFree(outImageData);
}


void gpuProcess(unsigned char* data, uint64_t dataLength, uint32_t pX, uint32_t pY, int iGPU)
{
        // Make sure the memory is allocated on the correct GPU 
        cudaSetDevice(iGPU);

	// MEMCPY to GPU
	unsigned char* deviceData;
	cudaMalloc((void**)&deviceData, sizeof(unsigned char) * dataLength);


	cudaMemcpy(deviceData, data, dataLength, cudaMemcpyHostToDevice);


	gpuSelector(deviceData, dataLength, pX, pY, iGPU);



	cudaMemcpy(data, deviceData, dataLength, cudaMemcpyDeviceToHost);

	cudaFree(deviceData);

	// Make sure GPU is in a good state!
	cudaDeviceReset();
}



// GPU COUNT
// Returns GPU Count

int gpuCount()
{
	int deviceCount;
	cudaGetDeviceCount(&deviceCount);
	return deviceCount;
}
