#include "BMP.h"
#include <fstream>

using namespace CTCBMP;

unsigned char* Bitmap::openBMP(const char* fileName)
{
	// Open the file!
	FILE* inFile;
	BMPFileHeader bmpFileHead;
	unsigned char* bmpImage;
	int imgIndx = 0;
	unsigned char tmpRGB;

	// Open_S
	errno_t fError = fopen_s(&inFile,fileName, "rb");
	if (fError != 0)
	{
		return NULL;
	}
	else
	{
		fread(&bmpFileHead, sizeof(BMPFileHeader),1,inFile);

		// Get BMP state
		if (bmpFileHead.bmpFileType != BMP_FILE_TYPE)
		{
			fclose(inFile);
			return NULL;
		}
		fread(&this->infoHeader,sizeof(BMPInfoHeader),1,inFile);
		// Move to the data!
		fseek(inFile,bmpFileHead.bmpFileOffBits,SEEK_SET);

		// ALLOC
		bmpImage = (unsigned char*)malloc(this->infoHeader.bmpInfoSizeImage);

		// Verify
		if (!bmpImage)
		{
			free(bmpImage);
			fclose(inFile);
			return NULL;
		}

		fread(bmpImage,this->infoHeader.bmpInfoSizeImage,1,inFile);

		// Verify AGAIN
		if (bmpImage == NULL)
		{
			free(bmpImage);
			fclose(inFile);
			return NULL;
		}

		// BITMAP ISN'T RGB, it is actually BGR :/
		/*for (imgIndx = 0; imgIndx < this->infoHeader.bmpInfoSizeImage; imgIndx += 3)
		{
			tmpRGB = bmpImage[imgIndx]; //Store the Blue channel
			bmpImage[imgIndx] = bmpImage[imgIndx + 2]; // Make the Image RGR
			bmpImage[imgIndx + 2] = tmpRGB; // Make it RGB
		}*/

		// close and return array

		bmpImage[1] = 0xff;

		fclose(inFile);
		return bmpImage;
	}
}
