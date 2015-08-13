#include "source.h"

#define THREADCOUNT 2

// Declare Mutex for Critical Regions
std::mutex gpuThread_mtx;

using namespace std;

void mainLine(char* file_name_str, int GPU)
{
	// TODO: DETECT MISSING FILES!

	// Load Image
	gpuThread_mtx.lock();
	FIBITMAP* _bitMap = FreeImage_Load(FreeImage_GetFileType(file_name_str), file_name_str);
	gpuThread_mtx.unlock();
	//FIBITMAP* _bitMap = FreeImage_Load(FIF_PNG, file_name_str);


	// Handle non 32bpp files
	if (FreeImage_GetBPP(_bitMap) != 32)
	{
		_bitMap = FreeImage_ConvertTo32Bits(_bitMap);
	}

	char* _outName = new char[256];
	strcpy_s(_outName, 250, file_name_str);
	strcat_s(_outName, 256, ".png");

	//Get Current image data - make a struct for this, it is awful
	uint32_t iWidth = FreeImage_GetWidth(_bitMap);
	uint32_t iHeight = FreeImage_GetHeight(_bitMap);
	/*_bitMap = FreeImage_Rescale(_bitMap, iWidth * 4, iHeight * 4);
	iWidth = FreeImage_GetWidth(_bitMap);
	iHeight = FreeImage_GetHeight(_bitMap);*/
	unsigned pitch = FreeImage_GetPitch(_bitMap);
	unsigned BPP = FreeImage_GetBPP(_bitMap);
	unsigned rMask = FreeImage_GetRedMask(_bitMap);
	unsigned gMask = FreeImage_GetGreenMask(_bitMap);
	unsigned bMask = FreeImage_GetBlueMask(_bitMap);

	unsigned char* imageData = FreeImage_GetBits(_bitMap);
	gpuProcess(imageData, iWidth * iHeight * 4, iWidth, iHeight,GPU);




	//FIBITMAP* preProc_bitmap = Immato_RunCustom(_bitMap, iWidth, iHeight, pitch, BPP, rMask, gMask, bMask);

	//FIBITMAP* finalOutput = FreeImage_Rescale(preProc_bitmap, iWidth / 2, iHeight / 2);

	//finalOutput = Immato_RunCustom(finalOutput, iWidth / 2, iHeight / 2, pitch, BPP, rMask, gMask, bMask);
	gpuThread_mtx.lock();
	FreeImage_Save(FIF_PNG, _bitMap, _outName);
	gpuThread_mtx.unlock();
	//FreeImage_CloseMemory();

	free(_outName);

	//delete[] file_name_str;
}



// Entry Point
// This is the program entry point!
// 
int main(int argc, char** argv)
{
	int devCount = gpuCount();
	if (devCount == 0)
	{
		cout << "IMMATO CUDA - This needs CUDA support" << endl << "Please Check current NVidia Drivers" << endl;
		return 1;
	}
	cout << devCount << " NV GPUS" << endl;


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
					if (jobsAllocated < argc - 1)
					{
						jobsAllocated++; // SEMAPHORE?
						cout << jobsRemaining << endl;
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
	}
	return 0;
}