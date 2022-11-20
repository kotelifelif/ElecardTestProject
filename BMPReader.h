#pragma once
#include <vector>
#include <string>

const int kBmpCode = 19778;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long int DWORD;
typedef long int LONG;
typedef long FXPT2DOT30;

struct CIEXYZ {
	FXPT2DOT30 ciexyzX;
	FXPT2DOT30 ciexyzY;
	FXPT2DOT30 ciexyzZ;
};

struct CIEXYZTRIPLE {
	CIEXYZ  ciexyzRed;
	CIEXYZ  ciexyzGreen;
	CIEXYZ  ciexyzBlue;
};

struct BITMAPFILEHEADER {
	WORD  bfType;
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
};

struct BITMAPV5HEADER {
	DWORD        bV5Size;
	LONG         bV5Width;
	LONG         bV5Height;
	WORD         bV5Planes;
	WORD         bV5BitCount;
	DWORD        bV5Compression;
	DWORD        bV5SizeImage;
	LONG         bV5XPelsPerMeter;
	LONG         bV5YPelsPerMeter;
	DWORD        bV5ClrUsed;
	DWORD        bV5ClrImportant;
	DWORD        bV5RedMask;
	DWORD        bV5GreenMask;
	DWORD        bV5BlueMask;
	DWORD        bV5AlphaMask;
	DWORD        bV5CSType;
	CIEXYZTRIPLE bV5Endpoints;
	DWORD        bV5GammaRed;
	DWORD        bV5GammaGreen;
	DWORD        bV5GammaBlue;
	DWORD        bV5Intent;
	DWORD        bV5ProfileData;
	DWORD        bV5ProfileSize;
	DWORD        bV5Reserved;
};

struct RGBFormat {
	BYTE rgbRed;
	BYTE rgbGreen;
	BYTE rgbBlue;
};

struct BMPFormat {
	BITMAPFILEHEADER bitmap{};
	BITMAPV5HEADER info{};
	std::vector<std::vector<RGBFormat>> rgbInfo;
};

class BMPReader
{
public:
	void WriteToFile(const std::string& file_name, BMPFormat& file_information);
	BMPFormat ReadFromFile(const std::string& file_name);
};
