#include "BMPReader.h"
#include "YUVReader.h"
#include <string>
#include <fstream>

using namespace std;

int main() {
	// CIF (352 x 288)
	string yuv_file{ "akiyo_cif.yuv" };
	string output_yuv_file{ "output_akiyo_cif.yuv" };
	string bmp_file{ "TV presenter 50.bmp" };
	YUVFormat yuv_video;
	YUVFormat yuv_picture;
	YUVFormat yuv_video_picture;
	BMPFormat bmp_file_information;
	YUVReader yuv_reader;
	BMPReader bmp_reader;

	yuv_video = yuv_reader.ReadFromFile(yuv_file, SizeFormat::CIF);
	bmp_file_information = bmp_reader.ReadFromFile(bmp_file);
	yuv_picture = yuv_reader.ConvertBmpToYuv(bmp_file_information, SizeFormat::CIF);
	yuv_video_picture = yuv_reader.AddPictureToVideo(yuv_video, yuv_picture);
	yuv_reader.WriteToFile(output_yuv_file, yuv_video_picture);
}