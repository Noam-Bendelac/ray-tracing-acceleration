// RayTracingCPU.cpp : Defines the entry point for the application.
//



#include "RayTracingCPU.h"
//#include <iostream>
//#include <fstream>

#include <fmt/core.h>
#include <Magick++.h>

using namespace std;


class File {
public:
	FILE* f;
	File(const char* s) : f(fopen(s, "w")) {}
	~File() {
		fclose(f);
	}
};


struct PngPixel {
	uint8_t rgb[3];
};


int main(int argc, char** argv)
{
	// Initialise ImageMagick library
	Magick::InitializeMagick(*argv);

	fmt::print("Hello\n");


	uint32_t width = 256;
	uint32_t height = 256;


	//uint8_t(*pix)[3] = new uint8_t[height * width][3];
	PngPixel* pix = new PngPixel[height * width];


	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			pix[width * y + x] = { uint8_t(255. * x / width), uint8_t(255. * y / height), 0 };
		}
	}

	// Create Image object and read in from pixel data above
	Magick::Image image;
	image.read(width, height, "RGB", Magick::CharPixel, pix);

	// Write the image to a file - change extension if you want a GIF or JPEG
	image.write("result.png");
	

	return 0;
}
