#include "BMPReader.h"
#include <fstream>
#include <iostream>

void BMPReader::WriteToFile(const std::string& file_name, BMPFormat& file_information)
{
	std::ofstream output_file_stream(file_name, std::ios_base::binary);

	if (file_information.bitmap.bfType != kBmpCode) {
		std::cout << "Input file is not bmp file" << std::endl;
		return;
	}

	output_file_stream.write(reinterpret_cast<char*>(&file_information.bitmap), sizeof(file_information.bitmap));
	output_file_stream.write(reinterpret_cast<char*>(&file_information.info), sizeof(file_information.info));

	for (int i = 0; i < file_information.info.bV5Height; i++) {
		for (int j = 0; j < file_information.info.bV5Width; j++) {
			output_file_stream.write(reinterpret_cast<char*>(&file_information.rgbInfo[i][j].rgbRed), sizeof(BYTE));
			output_file_stream.write(reinterpret_cast<char*>(&file_information.rgbInfo[i][j].rgbGreen), sizeof(BYTE));
			output_file_stream.write(reinterpret_cast<char*>(&file_information.rgbInfo[i][j].rgbBlue), sizeof(BYTE));
		}
	}
}

BMPFormat BMPReader::ReadFromFile(const std::string& file_name)
{
	std::ifstream input_file_stream(file_name, std::ios_base::binary);
	BMPFormat file_information;

	input_file_stream.read(reinterpret_cast<char*>(&file_information.bitmap), sizeof(file_information.bitmap));

	// It's not bmp file
	if (file_information.bitmap.bfType != kBmpCode) {
		std::cout << "Input file is not bmp file" << std::endl;
		return BMPFormat();
	}

	input_file_stream.read(reinterpret_cast<char*>(&file_information.info), sizeof(file_information.info));

	unsigned int bufer;

	for (int i = 0; i < file_information.info.bV5Height; i++) {
		file_information.rgbInfo.push_back(std::vector<RGBFormat>());
		for (int j = 0; j < file_information.info.bV5Width; ++j) {
			file_information.rgbInfo[i].push_back(RGBFormat());
		}
	}

	unsigned char* byte;
	input_file_stream.ignore(1024 - file_information.bitmap.bfOffBits);
	for (int i = 0; i < file_information.info.bV5Height; i++) {
		for (int j = 0; j < file_information.info.bV5Width; j++) {
			input_file_stream.read(reinterpret_cast<char*>(&bufer), file_information.info.bV5BitCount / 8);

			byte = (unsigned char*)&bufer;
			file_information.rgbInfo[file_information.info.bV5Height - i - 1][j].rgbGreen = *byte;
			++byte;
			file_information.rgbInfo[file_information.info.bV5Height - i - 1][j].rgbRed = *byte;
			++byte;
			file_information.rgbInfo[file_information.info.bV5Height - i - 1][j].rgbBlue = *byte;
		}
	}

	return file_information;
}