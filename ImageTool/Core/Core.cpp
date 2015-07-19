// Core.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BMP.h"


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Missing some args" << std::endl;
		std::cout << "Syntax;" << std::endl << "EXE <FILENAME>" << std::endl;
		return 1;
	}

	std::cout << "Running on file: " << argv[1] << std::endl << "Confirm?";
	getchar();

	// Malloc 512MB in 3Bytes - 536870912 / 3 = 178956972; OR WE CAN
	// Alloc NO Bytes * MB * 100
	unsigned char* imageData = (unsigned char*)malloc(3 * 1024 * 1024 * 100);

	CTCBMP::Bitmap* iMirage = new CTCBMP::Bitmap;
	imageData = iMirage->openBMP(argv[1]);

	if (imageData == NULL)
	{
		std::cout << "Oh Noes!" << std::endl;
		return 2;
	}

	// Pixel
	int pxl;
	for (pxl = 0; pxl < 100; pxl+=3)
	{
		printf("Pixel %d has values; \n", pxl);
		printf("Red = %x \n", imageData[(3 * pxl)]);
		printf("Green = %x \n", imageData[(3 * pxl) + 1]);
		printf("Blue = %x \n", imageData[(3 * pxl) + 2]);
	}
	free(imageData);
	return 0;
}

