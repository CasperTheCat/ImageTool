// Guard Rail

#ifndef __CTC_BMP__
#define __CTC_BMP__

#include <windows.h>

//#pragma pack(2)
typedef struct
{
	WORD bmpFileType;
	DWORD bmpFileSize;
	WORD bmpFileReservedA;
	WORD bmpFileReservedB;
	DWORD bmpFileOffBits;
} BMPFileHeader;
//#pragma pack()


#define BMP_FILE_TYPE 0x4D42

typedef struct
{
	DWORD bmpInfoSize;
	LONG bmpInfoWidth;
	LONG bmpInfoHeight;
	WORD bmpInfoPlanes;
	WORD bmpInfoBitCount;
	DWORD bmpInfoCompression;
	DWORD bmpInfoSizeImage;
	LONG bmpInfoPixelXPerMeter;
	LONG bmpInfoPixelYPerMeter;
	DWORD bmpInfoColors;
	DWORD bmpInfoColorsForce;
} BMPInfoHeader;

#define BMP_INFO_RGB 0
#define BMP_INFO_RLE8 1
#define BMP_INFO_RLE4 2
#define BMP_INFO_BITF 3

typedef struct
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserve;
} RGBQ;

namespace CTCBMP
{
	class Bitmap {
	public:
		unsigned char* openBMP(const char* filename);
		Bitmap();
		Bitmap(const char* filename);
		~Bitmap();


	private:
		//Do Things
	};
}
#endif
