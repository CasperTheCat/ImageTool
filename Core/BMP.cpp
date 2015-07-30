#include "BMP.h"
#include <fstream>

using namespace CTCBMP;
class Bitmap
{
	RGBQ* pixelData;
	BMPFileHeader fileHeader;
	BMPFileHeader bmpFileHead;
	BMPInfoHeader infoHeader;
	unsigned char *mArray;

	Bitmap()
	{
		
	}

	Bitmap(const char* fileName)
	{
		openBMP(fileName);
	}

	~Bitmap()
	{
		
	}

	void openBMP(const char* fileName)
	{
		// Open the file!
		FILE* inFile;
		
		unsigned char* bmpImage;
		auto imgIndx = 0;
		unsigned char tmpRGB;

		// Open_S
		auto fError = fopen_s(&inFile, fileName, "rb");
		if (fError != 0)
		{
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
			bmpImage = (unsigned char*)malloc(this->infoHeader.bmpInfoSizeImage);

			// Verify
			if (!bmpImage)
			{
				free(bmpImage);
				fclose(inFile);
				return;
			}

			fread(bmpImage, this->infoHeader.bmpInfoSizeImage, 1, inFile);

			// Verify AGAIN
			if (bmpImage == nullptr)
			{
				free(bmpImage);
				fclose(inFile);
				return;
			}

			// BITMAP ISN'T RGB, it is actually BGR :/
			for (imgIndx = 0; imgIndx < this->infoHeader.bmpInfoSizeImage; imgIndx += 3)
			{
				tmpRGB = bmpImage[imgIndx]; //Store the Blue channel
				bmpImage[imgIndx] = bmpImage[imgIndx + 2]; // Make the Image RGR
				bmpImage[imgIndx + 2] = tmpRGB; // Make it RGB
			}

			mArray = bmpImage;

			// close and return array

			//bmpImage[1] = 0xff;

			fclose(inFile);
		}
	}
};