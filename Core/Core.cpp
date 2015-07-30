// Core.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BMP.h"


int main(int argc, char* argv[])
{
	/*if (argc < 2)
	{
		std::cout << "Missing some args" << std::endl;
		std::cout << "Syntax;" << std::endl << "EXE <FILENAME>" << std::endl;
		return 1;
	}*/
	char* file_name_str = new char[MAX_PATH];
	if(argc < 2)
	{
		std::cout << "Please enter filename: ";
		std::cin >> file_name_str;
	} else
	{
		file_name_str = argv[1];
	}

	std::cout << "Running on file: " << file_name_str << std::endl << "Confirm?";
	getchar();

	// Malloc 512MB in 3Bytes - 536870912 / 3 = 178956972; OR WE CAN
	// Alloc NO Bytes * MB * 100
	unsigned char *imageData;// = static_cast<unsigned char*>(malloc(3 * 1024 * 1024 * 100));

	CTCBMP::Bitmap* iMirage = new CTCBMP::Bitmap();
	iMirage->openBMP(file_name_str);
	imageData = iMirage->getArray();


	if (imageData == nullptr)
	{
		std::cout << "Oh Noes! iMirage is " << iMirage->isInitialized << " vs " << false << std::endl;
		getchar();
		free(iMirage);
		return 2;
	}

	// Pixel
	int pxl;
	for (pxl = 0; pxl < 100; pxl+=3)
	{
		printf("Pixel %d has values; \n", (pxl / 3) );
		printf("Red = %x \n", imageData[(3 * pxl)]);
		printf("Green = %x \n", imageData[(3 * pxl) + 1]);
		printf("Blue = %x \n", imageData[(3 * pxl) + 2]);
	}
	//free(imageData);
	free(iMirage);
	getchar();
	return 0;
}
