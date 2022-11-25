#include "YUVReader.h"
#include <fstream>
#include <thread>

YUVFormat YUVReader::ReadFromFile(const std::string& input_file_name, SizeFormat format) {
	std::ifstream input_file_stream(input_file_name, std::ios_base::binary);
	YUVFormat file_information(format);

	input_file_stream.seekg(0, std::ios::end);
	int file_size = input_file_stream.tellg();
	input_file_stream.seekg(0, std::ios::beg);

	size_t pages_size = file_size * 2 / (3 * file_information.height * file_information.width);
	size_t y_size = (file_size * 2) / (3 * pages_size);
	size_t uv_size = ((file_size / pages_size) - y_size) / 2;

	file_information.pages.reserve(pages_size);
	BYTE value;
	for (int i = 0; i < pages_size; ++i) {
		Page page;
		page.Y.reserve(y_size);
		for (int j = 0; j < y_size; ++j) {
			input_file_stream.read(reinterpret_cast<char*>(&value), sizeof(BYTE));
			page.Y.push_back(value);
		}
		page.U.reserve(uv_size);
		for (int j = 0; j < uv_size; ++j) {
			input_file_stream.read(reinterpret_cast<char*>(&value), sizeof(BYTE));
			page.U.push_back(value);
		}
		page.V.reserve(uv_size);
		for (int j = 0; j < uv_size; ++j) {
			input_file_stream.read(reinterpret_cast<char*>(&value), sizeof(BYTE));
			page.V.push_back(value);
		}
		file_information.pages.push_back(page);
	}

	return file_information;
}


void YUVReader::WriteToFile(const std::string& file_name, YUVFormat& file_information)
{
	std::ofstream output_file_stream(file_name, std::ios_base::binary);
	int size = file_information.pages[0].Y.size();
	for (int i = 0; i < file_information.pages.size(); i++) {
		for (int j = 0; j < file_information.pages[i].Y.size(); j++) {
			output_file_stream.write(reinterpret_cast<char*>(&file_information.pages[i].Y[j]), sizeof(BYTE));
		}
		for (int j = 0; j < file_information.pages[i].U.size(); j++) {
			output_file_stream.write(reinterpret_cast<char*>(&file_information.pages[i].U[j]), sizeof(BYTE));
		}
		for (int j = 0; j < file_information.pages[i].V.size(); j++) {
			output_file_stream.write(reinterpret_cast<char*>(&file_information.pages[i].V[j]), sizeof(BYTE));
		}
	}
}

YUVFormat YUVReader::ConvertBmpToYuv(BMPFormat& bmp_format, SizeFormat format)
{
	YUVFormat file_information(format);

	const size_t kYUVCoefficient = 4;
	Page page;
	page.Y.reserve(file_information.height * file_information.width);
	page.U.reserve(file_information.height * file_information.width / kYUVCoefficient);
	page.V.reserve(file_information.height * file_information.width / kYUVCoefficient);

	const int kThreadSize = std::thread::hardware_concurrency();
	std::vector<std::thread> converter_threads;
	std::vector<Page> part_pages;
	part_pages.reserve(kThreadSize);
	converter_threads.reserve(kThreadSize);
	for (int i = 0; i < kThreadSize; i++) {
		Page part_page;
		part_page.Y.reserve(file_information.height * file_information.width / kThreadSize);
		part_page.U.reserve(file_information.height * file_information.width / (4 * kThreadSize));
		part_page.V.reserve(file_information.height * file_information.width / (4 * kThreadSize));
		part_pages.push_back(part_page);
		int start = i * file_information.height / kThreadSize;
		int finish = (i + 1) * file_information.height / kThreadSize;
		std::thread converter_thread(&YUVReader::FillPage, this, start, finish, std::ref(part_pages[i]), std::ref(file_information), std::ref(bmp_format));
		converter_threads.push_back(move(converter_thread));
	}

	for (int i = 0; i < kThreadSize; i++) {
		converter_threads[i].join();
		int start = i * file_information.height * file_information.width / kThreadSize;
		page.Y.insert(page.Y.begin() + start, part_pages[i].Y.begin(), part_pages[i].Y.end());
		page.U.insert(page.U.begin() + start / kYUVCoefficient, part_pages[i].U.begin(), part_pages[i].U.end());
		page.V.insert(page.V.begin() + start / kYUVCoefficient, part_pages[i].V.begin(), part_pages[i].V.end());
	}

	file_information.pages.push_back(page);

	return file_information;
}

YUVFormat YUVReader::AddPictureToVideo(YUVFormat& video, YUVFormat& picture)
{
	if ((video.height < picture.height) || (video.width < picture.width)) {
		return YUVFormat{};
	}

	YUVFormat video_picture{ video };

	int size = video_picture.pages[0].Y.size();
	for (int i = 0; i < video.pages.size(); i++) {
		for (int j = 0; j < picture.pages[0].Y.size(); j++) {
			if (picture.pages[0].Y[j] == 0)
				continue;
			video_picture.pages[i].Y[j] = picture.pages[0].Y[j];
		}
		for (int j = 0; j < picture.pages[0].U.size(); j++) {
			if (picture.pages[0].U[j] == 0)
				continue;
			video_picture.pages[i].U[j] = picture.pages[0].U[j];
		}
		for (int j = 0; j < picture.pages[0].V.size(); j++) {
			if (picture.pages[0].V[j] == 0)
				continue;
			video_picture.pages[i].V[j] = picture.pages[0].V[j];
		}
	}

	return video_picture;
}

void YUVReader::FillPage(const int start, const int finish, Page& page, YUVFormat& file_information, BMPFormat& bmp_format)
{
	int y_value;
	int u_value;
	int v_value;
	for (int i = start; i < finish; i++) {
		for (int j = 0; j < file_information.width; j++) {
			if (i < bmp_format.info.bV5Height && j < bmp_format.info.bV5Width) {
				y_value = 0.257 * bmp_format.rgbInfo[i][j].rgbRed + 0.504 * bmp_format.rgbInfo[i][j].rgbGreen + 0.098 * bmp_format.rgbInfo[i][j].rgbBlue + 16;
				u_value = -0.148 * bmp_format.rgbInfo[i][j].rgbRed - 0.291 * bmp_format.rgbInfo[i][j].rgbGreen + 0.439 * bmp_format.rgbInfo[i][j].rgbBlue + 128;
				v_value = 0.439 * bmp_format.rgbInfo[i][j].rgbRed - 0.368 * bmp_format.rgbInfo[i][j].rgbGreen - 0.071 * bmp_format.rgbInfo[i][j].rgbBlue + 128;
			}
			else {
				y_value = 0;
				u_value = 0;
				v_value = 0;
			}
			page.Y.push_back(y_value);

			if (j % 2 == 0 && i % 2 == 0)
			{
				page.U.push_back(u_value);
				page.V.push_back(v_value);
			}
		}
	}
}
