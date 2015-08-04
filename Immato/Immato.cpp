// Immato.cpp : Defines the entry point for the console application.
//https://software.intel.com/en-us/articles/setting-up-the-build-environment-for-using-intel-c-or-fortran-compilers
//

#include "stdafx.h"
//Define the luma for sRGB


using namespace std;



inline int Modulus(int a, int b)
{
	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

/*bool isf3Bigger(float3 fX, float3 fY)
{
	if(fX - fY > 0.f)
}*/

inline float dot(float3 iX, float3 iY)
{
	return (iX.R * iY.R) + (iX.G * iY.G) + (iX.B * iY.B);
}

inline float dot(float iX, float3 iY)
{
	return (iX * iY.R) + (iX * iY.G) + (iX * iY.B);
}

inline float dot(float4 iX, float3 iY)
{
	return (iX.R * iY.R) + (iX.G * iY.G) + (iX.B * iY.B);
}


inline float vMag(float3 vX)
{
	return sqrt(dot(vX, vX));
}


inline float CharToFloat(unsigned char in)
{
	return (float(in) / float(255));
}

inline unsigned char FloatToChar(float in)
{
	return unsigned char(Immato_Clamp(in) * 255);
}

FIBITMAP* Immato_RunCustom(FIBITMAP* image, uint32_t pW, uint32_t pH, int pitch, unsigned BPP, unsigned rMask, unsigned gMask, unsigned bMask)
{
	//Get Raw Data
	unsigned char* imageData = FreeImage_GetBits(image);

	// MEMORY WHORING make a bVec3
	float4* bmpData = new float4[pH*pW]; // Size is pW*pH*4

	uint64_t bytsInImg = pW * pH * 4;

	// Convert to float4
#pragma omp parallel for
	for (long iP = 0; iP < bytsInImg; iP += 4)
	{
		bmpData[iP / 4].R = CharToFloat(imageData[iP]);
		bmpData[iP / 4].G = CharToFloat(imageData[iP + 1]);
		bmpData[iP / 4].B = CharToFloat(imageData[iP + 2]);
		bmpData[iP / 4].A = CharToFloat(imageData[iP + 3]);
	}
	cout << "Image Decoded" << endl;
	// Image manip loop

	float3 LUMA{ float(0.2126), float(0.7152), float(0.0722) };



	// Get our luminos
	float* lArray = new float[pH*pW];
	float3* cArray = new float3[pH*pW];
#pragma omp parallel for
	for (long iC = 0; iC < bytsInImg / 4; iC++)
	{
		//float lumin;
		// Dot Product!!
		//lArray[iC] = (bmpData[iC].R*LUMA.R) + (bmpData[iC].G*LUMA.G) + (bmpData[iC].B*LUMA.B);

		lArray[iC] = dot(bmpData[iC], LUMA);
		cArray[iC] = bmpData[iC].RGB() - lArray[iC];

	}

	cout << "Luminosity and Chroma Step Complete" << endl;




	// Calc out the dif - BLURSHARP
	float* difArr = new float[pH*pW];
#pragma omp parallel for
	for (long iC = 0; iC < bytsInImg / 4; iC++)
	{/*
		BLUR 1:
		[00,00,00,00,00
		 00,11,11,11,00
		 00,11,12,11,00
		 00,11,11,11,00
		 00,00,00,00,00]

		 BLUR 2:
		 [00,00,00,00,00
		  00,08,14,08,00
		  00,14,12,14,00
		  00,08,14,08,00
		  00,00,00,00,00]
		  */
		  // Immediate Weights
		float difUp = lArray[Modulus((iC - pW), (bytsInImg / 4))];
		float difDown = lArray[Modulus((iC + pW), (bytsInImg / 4))];
		float difLeft = lArray[Modulus((iC - 1), (bytsInImg / 4))];
		float difRight = lArray[Modulus((iC + 1), (bytsInImg / 4))];

		// Non Immed
		float difUpRight = lArray[Modulus(((iC - pW) + 1), (bytsInImg / 4))];
		float difDownLeft = lArray[Modulus(((iC + pW) - 1), (bytsInImg / 4))];
		float difUpLeft = lArray[Modulus(((iC - pW) - 1), (bytsInImg / 4))];
		float difDownRight = lArray[Modulus(((iC + pW) + 1), (bytsInImg / 4))];

		difArr[iC] = lArray[iC] * 0.12f
			+ difUp * 0.14f
			+ difDown * 0.14f
			+ difLeft * 0.14f
			+ difRight * 0.14f
			+ difUpRight * 0.08f
			+ difUpLeft * 0.08f
			+ difDownLeft * 0.08f
			+ difDownRight * 0.08f;
	}

	cout << "Luminosity Sharp Mask Complete" << endl;


// REDUCE THE CHROMA DISTORT!
#pragma omp parallel for
	for (long iC = 0; iC < bytsInImg / 4; iC++)
	{
		/*
		BLUR X:
		[  ,  2,  3,   ,
		   ,  8, 12,  8, 2
		  3, 12,   , 12, 3
		  2,  8, 12,  8,
		   ,   ,  3,  2,  ]
		*/
		// LATERAL
		float3 dif3Up = cArray[Modulus((iC - pW), (bytsInImg / 4))];
		float3 dif3Down = cArray[Modulus((iC + pW), (bytsInImg / 4))];
		float3 dif3Left = cArray[Modulus((iC - 1), (bytsInImg / 4))];
		float3 dif3Right = cArray[Modulus((iC + 1), (bytsInImg / 4))];

		// DIAGONAL
		float3 dif3UpRight = cArray[Modulus(((iC - pW) + 1), (bytsInImg / 4))];
		float3 dif3DownLeft = cArray[Modulus(((iC + pW) - 1), (bytsInImg / 4))];
		float3 dif3UpLeft = cArray[Modulus(((iC - pW) - 1), (bytsInImg / 4))];
		float3 dif3DownRight = cArray[Modulus(((iC + pW) + 1), (bytsInImg / 4))];

		// LATERAL Out
		float3 dif3UpUp = cArray[Modulus((iC - 2 * pW), (bytsInImg / 4))];
		float3 dif3DownDown = cArray[Modulus((iC + 2 * pW), (bytsInImg / 4))];
		float3 dif3LeftLeft = cArray[Modulus((iC - 2), (bytsInImg / 4))];
		float3 dif3RightRight = cArray[Modulus((iC + 2), (bytsInImg / 4))];

		// DIAGONAL with outward shift
		float3 dif3UpUpLeft = cArray[Modulus((iC - 2 * pW) - 1, (bytsInImg / 4))];
		float3 dif3DownDownRight = cArray[Modulus((iC + 2 * pW) + 1, (bytsInImg / 4))];
		float3 dif3LeftLeftDown = cArray[Modulus((iC - 2) + pW, (bytsInImg / 4))];
		float3 dif3RightRightUp = cArray[Modulus((iC + 2) - pW, (bytsInImg / 4))];

		cArray[iC] = dif3Up * 0.12f
			+ dif3Down * 0.12f
			+ dif3Left * 0.12f
			+ dif3Right * 0.12f // mid

			+ dif3UpRight * 0.08f
			+ dif3UpLeft * 0.08f
			+ dif3DownLeft * 0.08f
			+ dif3DownRight * 0.08f

			+ dif3UpUp * 0.03f
			+ dif3DownDown * 0.03f
			+ dif3LeftLeft * 0.03f
			+ dif3RightRight * 0.03f

			+ dif3UpUpLeft * 0.02f
			+ dif3DownDownRight * 0.02f
			+ dif3LeftLeftDown * 0.02f
			+ dif3RightRightUp * 0.02f;
	}

	cout << "Chroma Smooth Complete" << endl;

	uint64_t kliTriggers = 0;
#pragma omp parallel for
	for (long iX = 0; iX < bytsInImg / 4; iX++)
	{
		if (iX == 0)
		{
			cout << vMag(cArray[iX]) << ".. VS .." << vMag(cArray[Modulus(iX + 1, bytsInImg / 4)]) * 1.25 << endl;
		}

		int fMover = 1;

		// Bring PixelAlignment
		while ((vMag(cArray[iX]) < vMag(cArray[Modulus(iX + fMover,bytsInImg / 4)]) * 1.05 	&& vMag(cArray[iX]) > vMag(cArray[Modulus(iX + fMover, bytsInImg / 4)]) * 0.95)
			&& fMover < 4)
		{
			cArray[Modulus(iX + fMover, bytsInImg / 4)] = (cArray[Modulus(iX + fMover, bytsInImg / 4)] + (cArray[iX] + cArray[Modulus(iX + fMover, bytsInImg / 4)]) / 2) / 2;
			fMover++;
			kliTriggers++;
			
		}
		fMover = 1;
		while ((vMag(cArray[iX]) < vMag(cArray[Modulus(iX - fMover, bytsInImg / 4)]) * 1.05 && vMag(cArray[iX]) > vMag(cArray[Modulus(iX - fMover, bytsInImg / 4)]) * 0.95)
			&& fMover < 4)
		{
			cArray[Modulus(iX - fMover, bytsInImg / 4)] = (cArray[Modulus(iX - fMover, bytsInImg / 4)] + (cArray[iX] + cArray[Modulus(iX - fMover, bytsInImg / 4)]) / 2) / 2;
			fMover++;
			kliTriggers++;
		}

		
	}

	cout << "Pixel Alignment adjusted" << kliTriggers << " pixels" << endl;

	getchar();

	float edgeThreshold = 0.01f;
#pragma omp parallel for
	for (long iC = 0; iC < bytsInImg / 4; iC++)
	{
		//float3 chroma = bmpData[iC].RGB() - (lArray[iC]);

		//chroma = (((chroma - 0.5f) * 1.2f) + 0.5f);

		float sharpMask = lArray[iC] - difArr[iC];

		//sharpMask = round(Immato_Clamp((((sharpMask - edgeThreshold) * 10.f) + edgeThreshold)));
		sharpMask = (round(sharpMask * 64.f)) / 64.f;

		bmpData[iC] = cArray[iC] + lArray[iC] + (dot(sharpMask, LUMA));

		/*bmpData[iC].R =3 * (dot(sharpMask, LUMA));
		bmpData[iC].G =3 * (dot(sharpMask, LUMA));
		bmpData[iC].B =3 * (dot(sharpMask, LUMA));*/

		/*bmpData[iC].R = sharpMask;// *lArray[iC]);
		bmpData[iC].G = sharpMask;// * lArray[iC]);
		bmpData[iC].B = sharpMask;// * lArray[iC]);*/


		//bmpData[iC] -= 0.5f;
		//bmpData[iC] *= 1.2f;
		//bmpData[iC] += 0.5f;
	}
	cout << "Image Processed" << endl;


	unsigned char* iDat = new unsigned char[bytsInImg];

#pragma omp parallel for
	for (long iP = 0; iP < bytsInImg; iP += 4)
	{
		/*iDat[iP] = FloatToChar(bmpData[iP / 4].R);
		iDat[iP + 1] = FloatToChar(bmpData[iP / 4].G);
		iDat[iP + 2] = FloatToChar(bmpData[iP / 4].B);
		iDat[iP + 3] = FloatToChar(bmpData[iP / 4].A);*/

		iDat[iP] = FloatToChar(round(bmpData[iP / 4].R * 127.f) / 127.f);
		iDat[iP + 1] = FloatToChar(round(bmpData[iP / 4].G * 127.f) / 127.f);
		iDat[iP + 2] = FloatToChar(round(bmpData[iP / 4].B * 127.f) / 127.f);
		iDat[iP + 3] = FloatToChar(round(bmpData[iP / 4].A * 127.f) / 127.f);
	}

	/* This moves left 50% to right side
	#pragma omp parallel for
		for (long iP = 0; iP < bytsInImg; iP += 4)
		{
			if (iP < bytsInImg  - 4)
			{
				iDat[iP] = FloatToChar(bmpData[((iP / 4) + pW / 2) % (bytsInImg / 4)].R);
				iDat[iP + 1] = FloatToChar(bmpData[((iP / 4) + pW / 2) % (bytsInImg / 4)].G);
				iDat[iP + 2] = FloatToChar(bmpData[((iP / 4) + pW / 2) % (bytsInImg / 4)].B);
				iDat[iP + 3] = FloatToChar(bmpData[((iP / 4) + pW / 2) % (bytsInImg / 4)].A);
			}
		}*/

	cout << "Image Encoded" << endl;

	delete[] lArray;
	delete[] cArray;
	delete[] difArr;
	delete[] bmpData;

	FIBITMAP* returnVal = FreeImage_ConvertFromRawBits(iDat, pW, pH, pitch, BPP, rMask, gMask, bMask);

	delete[] iDat;

	// We has data!
	return returnVal;

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
	_bitMap = FreeImage_Rescale(_bitMap, iWidth * 4, iHeight * 4);
	iWidth = FreeImage_GetWidth(_bitMap);
	iHeight = FreeImage_GetHeight(_bitMap);
	unsigned pitch = FreeImage_GetPitch(_bitMap);
	unsigned BPP = FreeImage_GetBPP(_bitMap);
	unsigned rMask = FreeImage_GetRedMask(_bitMap);
	unsigned gMask = FreeImage_GetGreenMask(_bitMap);
	unsigned bMask = FreeImage_GetBlueMask(_bitMap);


	// DO SOME HLSL!
	FIBITMAP* preProc_bitmap = Immato_RunCustom(_bitMap, iWidth, iHeight, pitch, BPP, rMask, gMask, bMask);

	FIBITMAP* finalOutput = FreeImage_Rescale(preProc_bitmap, iWidth / 2, iHeight / 2);

	//finalOutput = Immato_RunCustom(finalOutput, iWidth / 2, iHeight / 2, pitch, BPP, rMask, gMask, bMask);

	FreeImage_Save(FIF_BMP, finalOutput, "TEST.bmp");

	//delete(_outName);
	//delete[] file_name_str;
	return 0;
}

