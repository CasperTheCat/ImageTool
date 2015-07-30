#include "BMP.h"
#include <fstream>
#include <iostream>

using namespace CTCBMP;



unsigned char* CTCBMP::Bitmap::getArray()
{
	if (isInitialized)
	{
		return mArray;
	}
	return nullptr;
}

CTCBMP::Bitmap::Bitmap()
{
	return;
}

CTCBMP::Bitmap::Bitmap(const char* fileName)
{
	openBMP(fileName);
}

CTCBMP::Bitmap::~Bitmap()
{
	if (isInitialized)
	{
		delete(mArray);
	}
}

void CTCBMP::Bitmap::openBMP(const char* fileName)
{
	// Open the file!
	FILE* inFile;

	//unsigned char* mArray;
	auto imgIndx = 0;
	unsigned char tmpRGB;

	// Open_S
	auto fError = fopen_s(&inFile, fileName, "rb");
	if (fError != 0)
	{
		std::cout << "Invalid File" << std::endl;
		return;
	}
	else
	{
		fread(&bmpFileHead, sizeof(BMPFileHeader), 1, inFile);

		// Get BMP state
		if (bmpFileHead.bmpFileType != BMP_FILE_TYPE)
		{
			fclose(inFile);
			return;
		}
		fread(&this->infoHeader, sizeof(BMPInfoHeader), 1, inFile);
		// Move to the data!
		fseek(inFile, bmpFileHead.bmpFileOffBits, SEEK_SET);

		// ALLOC
		mArray = static_cast<unsigned char*>(malloc(this->infoHeader.bmpInfoSizeImage));

		// Verify
		if (!mArray)
		{
			free(mArray);
			fclose(inFile);
			std::cout << "Memory Allocation failure" << std::endl;
			return;
		}

		fread(mArray, this->infoHeader.bmpInfoSizeImage, 1, inFile);

		// Verify AGAIN
		if (mArray == nullptr)
		{
			free(mArray);
			std::cout << " Disk to Memory failure" << std::endl;
			fclose(inFile);
			return;
		}

		// BITMAP ISN'T RGB, it is actually BGR :/
		for (imgIndx = 0; imgIndx < this->infoHeader.bmpInfoSizeImage; imgIndx += 3)
		{
			tmpRGB = mArray[imgIndx]; //Store the Blue channel
			mArray[imgIndx] = mArray[imgIndx + 2]; // Make the Image RGR
			mArray[imgIndx + 2] = tmpRGB; // Make it RGB
		}

		//mArray = mArray;
		//free(mArray);

		// close and return array

		//mArray[1] = 0xff;

		fclose(inFile);
		isInitialized = true;
	}
}