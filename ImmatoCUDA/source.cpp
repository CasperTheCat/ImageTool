#include "source.h"

using namespace std;

void mainLine(char* file_name_str)
{
	// TODO: DETECT MISSING FILES!

	// Load Image
	FIBITMAP* _bitMap = FreeImage_Load(FreeImage_GetFileType(file_name_str), file_name_str);
	//FIBITMAP* _bitMap = FreeImage_Load(FIF_PNG, file_name_str);


	// Handle non 32bpp files
	if (FreeImage_GetBPP(_bitMap) != 32)
	{
		_bitMap = FreeImage_ConvertTo32Bits(_bitMap);
	}

	char* _outName = new char[256];
	strcpy_s(_outName, 250, file_name_str);
	strcat_s(_outName, 256, ".png");

	//Get Current shiz - make a struct for this, it is awful
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
	gpuProcess(imageData, iWidth * iHeight * 4, iWidth, iHeight);




	//FIBITMAP* preProc_bitmap = Immato_RunCustom(_bitMap, iWidth, iHeight, pitch, BPP, rMask, gMask, bMask);

	//FIBITMAP* finalOutput = FreeImage_Rescale(preProc_bitmap, iWidth / 2, iHeight / 2);

	//finalOutput = Immato_RunCustom(finalOutput, iWidth / 2, iHeight / 2, pitch, BPP, rMask, gMask, bMask);

	FreeImage_Save(FIF_PNG, _bitMap, _outName);
	//FreeImage_CloseMemory();

	//delete(_outName);
	//delete[] file_name_str;
}

int main(int argc, char** argv)
{
	char* file_name_str = new char[250];
	if (argc < 2)
	{
		cout << "Please enter filename: ";
		cin >> file_name_str;
		mainLine(file_name_str);
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			file_name_str = argv[i];
			mainLine(file_name_str);
			std::cout << file_name_str << " processed" << std::endl;
		}
	}


	return 0;
}