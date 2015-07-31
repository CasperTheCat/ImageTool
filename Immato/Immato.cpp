// Immato.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


using namespace std;

float CharToFloat(unsigned char in)
{
	return (float(in) / float(255));
}

unsigned char FloatToChar(float in)
{
	return unsigned char(Immato_Clamp(in)*255);
}

FIBITMAP* Immato_RunCustom(FIBITMAP* image, uint32_t pW, uint32_t pH , int pitch, unsigned BPP, unsigned rMask, unsigned gMask, unsigned bMask)
{
	//Get Raw Data
	unsigned char* imageData = FreeImage_GetBits(image);

	// MEMORY WHORING make a bVec3
	bVec3* bmpData = new bVec3[pH*pW]; // Size is pW*pH*4

	uint64_t bytsInImg = pW * pH * 4;


	for (uint64_t iP = 0; iP < bytsInImg; iP += 4)
	{
		bmpData[iP / 4].R = imageData[iP];
		bmpData[iP / 4].G = imageData[iP + 1];
		bmpData[iP / 4].B = imageData[iP + 2];
		bmpData[iP / 4].A = imageData[iP + 3];
	}

	unsigned char* iDat = new unsigned char[bytsInImg];

	for (uint64_t iP = 0; iP < bytsInImg; iP += 4)
	{
		iDat[iP] = bmpData[iP / 4].R;
		iDat[iP + 1] = bmpData[iP / 4].G;
		iDat[iP + 2] = bmpData[iP / 4].B;
		iDat[iP + 3] = bmpData[iP / 4].A;
	}

#pragma omp parallel for
	for (long iC = 0; iC < bytsInImg; iC++)
	{
		float _nDat = CharToFloat(iDat[iC]);
		_nDat -= 0.5f;
		_nDat *= 1.2f;
		_nDat += 0.5f;
		iDat[iC] = FloatToChar(_nDat);
	}

	// We has data!
	return FreeImage_ConvertFromRawBits(iDat, pW, pH, pitch, BPP, rMask, gMask, bMask);

	//return _outImage;
}

int main(int argc, char** argv)
{
	char* file_name_str = new char[250];
	if (argc < 2)
	{
		cout << "Please enter filename: ";
		cin >> file_name_str;
	}
	else
	{
		file_name_str = argv[1];
	}

	// TODO: DETECT MISSING FILES!

	// Load Image
	FIBITMAP* _bitMap = FreeImage_Load(FreeImage_GetFileType(file_name_str), file_name_str);
	//FIBITMAP* _bitMap = FreeImage_Load(FIF_PNG, file_name_str);


	// Handle non 32bpp files
	if (FreeImage_GetBPP(_bitMap) != 32)
	{
		_bitMap = FreeImage_ConvertTo32Bits(_bitMap);
	}

	//char* _outName = new char[256];
	//strcpy_s(_outName,250,file_name_str);
	//strcat_s(_outName, 6, ".CTC");

	//Get Current shiz - make a struct for this, it is awful
	uint32_t iWidth = FreeImage_GetWidth(_bitMap);
	uint32_t iHeight = FreeImage_GetHeight(_bitMap);
	_bitMap = FreeImage_Rescale(_bitMap, iWidth * 2, iHeight * 2);
	iWidth = FreeImage_GetWidth(_bitMap);
	iHeight = FreeImage_GetHeight(_bitMap);
	unsigned pitch = FreeImage_GetPitch(_bitMap);
	unsigned BPP = FreeImage_GetBPP(_bitMap);
	unsigned rMask = FreeImage_GetRedMask(_bitMap);
	unsigned gMask = FreeImage_GetGreenMask(_bitMap);
	unsigned bMask = FreeImage_GetBlueMask(_bitMap);


	// DO SOME HLSL!
	FIBITMAP* rekt_fibitmap = Immato_RunCustom(_bitMap, iWidth, iHeight, pitch, BPP, rMask, gMask, bMask);

	FreeImage_Save(FIF_BMP,rekt_fibitmap , "TEST.bmp");

	//delete(_outName);
	//delete[] file_name_str;
	return 0;
}

