#include "source.h"

#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f
#define THREADCOUNT 2

// Declare Mutex for Critical Regions
std::mutex gpuThread_mtx;

using namespace std;

void mainLine(char* file_name_str, int GPU)
{
	// TODO: DETECT MISSING FILES!

	// Load Image
	//gpuThread_mtx.lock();
	// 

	// BitMap Pointers
	// oBitMap = Original Load
	// convBitMap = convertedBitMap
	FIBITMAP* convBitMap;
	FIBITMAP* oBitMap = FreeImage_Load(FreeImage_GetFileType(file_name_str), file_name_str);

	// Convert and Free Old resources
	convBitMap = FreeImage_ConvertTo32Bits(oBitMap);
	FreeImage_Unload(oBitMap);

	//gpuThread_mtx.unlock();
	//FIBITMAP* _bitMap = FreeImage_Load(FIF_PNG, file_name_str);



	/*// Handle non 32bpp files
	if (FreeImage_GetBPP(oBitMap) != 32)
	{
		convBitMap = FreeImage_ConvertTo32Bits(oBitMap);
		FreeImage_Unload(oBitMap);
	}*/


	char* _outName = new char[256];
	strcpy_s(_outName, 250, file_name_str);
	strcat_s(_outName, 256, "_p.png");

	//Get Current image data - make a struct for this, it is awful
	uint32_t iWidth = FreeImage_GetWidth(convBitMap);
	uint32_t iHeight = FreeImage_GetHeight(convBitMap);
	/*_bitMap = FreeImage_Rescale(_bitMap, iWidth * 4, iHeight * 4);
	iWidth = FreeImage_GetWidth(_bitMap);
	iHeight = FreeImage_GetHeight(_bitMap);*/
	/*unsigned pitch = FreeImage_GetPitch(_bitMap);
	unsigned BPP = FreeImage_GetBPP(_bitMap);
	unsigned rMask = FreeImage_GetRedMask(_bitMap);
	unsigned gMask = FreeImage_GetGreenMask(_bitMap);
	unsigned bMask = FreeImage_GetBlueMask(_bitMap);*/

	unsigned char* imageData = FreeImage_GetBits(convBitMap);

	// OCL Setup
	ocl_args_d_t ocl;

	// OCL Launch
	gpuProcess(&ocl,imageData, iWidth * iHeight * 4, iWidth, iHeight,GPU);




	//FIBITMAP* preProc_bitmap = Immato_RunCustom(_bitMap, iWidth, iHeight, pitch, BPP, rMask, gMask, bMask);

	//FIBITMAP* finalOutput = FreeImage_Rescale(preProc_bitmap, iWidth / 2, iHeight / 2);

	//finalOutput = Immato_RunCustom(finalOutput, iWidth / 2, iHeight / 2, pitch, BPP, rMask, gMask, bMask);
	//gpuThread_mtx.lock();
	FreeImage_Save(FIF_PNG, convBitMap, _outName);
	//gpuThread_mtx.unlock();
	//FreeImage_CloseMemory();
	FreeImage_Unload(convBitMap);
	//free(_bitMap);
	//free(imageData);

	free(_outName);

	//delete[] file_name_str;
}

// Load Bar
// Loading Bar Progress
// 
static inline void loadBar(int cLoop, int endLoop)
{

	// Calculate the ratio of complete-to-incomplete.
	float ratio = cLoop / (float)endLoop;

	// Print
	cout << cLoop << "/" << endLoop << "\t : " << std::setprecision(3) << ratio * 100 << "%\t\t\t\r";
}


/*// Entry Point
// This is the program entry point!
// 
int main(int argc, char** argv)
{
	int devCount = gpuCount();
	if (devCount == 0)
	{
		cout << "IMMATO OpenCL - This needs OpenCL support" << endl << "Please Check current device support" << endl;
		return 1;
	}
	cout << devCount << " OCL Devices" << endl;


	char* file_name_str = new char[250];
	if (argc < 2)
	{
		cout << "Please enter filename: ";
		cin >> file_name_str;
		mainLine(file_name_str, 0);
	}
	else
	{
		// MultiGPU conflict test
		//--devCount;


		// Test CUDA Thread Support
		// Create max 4 threads for testing on a dual GPU system
		//
		
		// Job Queue and Feed
		// Remaining
		// Feed is for storing job states
		int jobsRemaining = argc - 1;
		int jobsAllocated = 0;
		bool* _gpuAlloc = new bool[devCount];

		// Thread Pool
		// Works as a container of pointers
		thread** tPool = new thread*[devCount];

		// Init the first devCount jobs		
		for (int tJ = 0; tJ < devCount && tJ < jobsRemaining; tJ++)
		{
			tPool[tJ] = new thread(mainLine, argv[tJ+1], tJ);
			_gpuAlloc[tJ] = true;
			jobsAllocated++;
			cout << "Thread " << tJ << " using GPU " << tJ << endl;
		}
		cout << endl;

		// Loop until all done. We are off by one...
		// I don't even
		while (jobsRemaining > 0)
		{
			// For every active thread
			// Check if joinable, if they are then
			// Join and create the next job!
			for (int cT = 0; cT < devCount && cT <= jobsRemaining; cT++)
			{
				if (tPool[cT]->joinable())
				{
					tPool[cT]->join();
					--jobsRemaining;
					//cout << "File " << jobsAllocated - devCount + 1<< " completed" << endl;
					loadBar(jobsAllocated - devCount + 1, argc - 1);
					if (jobsAllocated < argc - 1)
					{
						jobsAllocated++;
						tPool[cT] = new thread(mainLine, argv[jobsAllocated], cT);
					}
				}
			}
		}
		



		/* This is unused due to threading implementation
		 * Kept for reference
		 * 
		for (int i = 1; i < argc; i++)
		{
			file_name_str = argv[i];
			mainLine(file_name_str);
			std::cout << file_name_str << " processed" << std::endl;
		}*/
	/*}
	return 0;
}*/