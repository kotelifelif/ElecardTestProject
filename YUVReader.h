#pragma once
#include "BMPReader.h"

#include <string>
#include <vector>

typedef unsigned char BYTE;

enum class SizeFormat {
	CIF,
	QCIF
};

struct Page {
	std::vector<BYTE> Y;
	std::vector<BYTE> U;
	std::vector<BYTE> V;
};

struct YUVFormat {
	YUVFormat() : width(352), height(288) {

	};

	YUVFormat(SizeFormat format) {
		if (format == SizeFormat::CIF) {
			width = 352;
			height = 288;
		}
		else {
			width = 176;
			height = 144;
		}
	}
	size_t width;
	size_t height;
	size_t bitmap_width = 0;
	size_t bitmap_height = 0;
	std::vector<Page> pages;
};

class YUVReader
{
public:
	YUVFormat ReadFromFile(const std::string& input_file_name, SizeFormat format);
	void WriteToFile(const std::string& file_name, YUVFormat& file_information);
	YUVFormat ConvertBmpToYuv(BMPFormat& bmp_format, SizeFormat format);
	YUVFormat AddPictureToVideo(YUVFormat& video, YUVFormat& picture);
private:
	void FillPage(const int start, const int finish, Page& page, YUVFormat& file_information, BMPFormat& bmp_format);
};

