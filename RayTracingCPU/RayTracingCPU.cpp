// RayTracingCPU.cpp : Defines the entry point for the application.
//



#include "RayTracingCPU.h"
#include "Renderer.h"

#include <fmt/core.h>
#include <Magick++.h>
#include <glm/glm.hpp>
#include <memory>
#include <cstdlib>
#include <chrono>
#include <cmath>



class File {
public:
	FILE* f;
	File(const char* s) : f(fopen(s, "w")) {}
	~File() {
		fclose(f);
	}
};


struct PngPixel {
	/*uint8_t rgb[3];*/
	glm::u8vec3 rgb;
	PngPixel() : rgb() {}
	explicit PngPixel(glm::vec3 f_rgb) : rgb(f_rgb) {}
};


float randF() {
	return float(rand()) / RAND_MAX;
}

void initScene(Scene& s, uint32_t numEntities) {
	auto mat = new DiffuseMaterial{ glm::vec3{1,1,0} };
	
	for (uint32_t i = 0; i < numEntities; ++i) {
		s.entities.push_back(std::make_unique<Sphere>(
			glm::vec3{ randF() * 16. - 8., randF() * 16. - 8., randF() * 10. - 2. },
			4.f/powf(numEntities, 1.0f/3),
			*mat
		));
	}

	s.entities.push_back(std::make_unique<Sphere>(
		glm::vec3{ 0, 0, -200 },
		200.f,
		*mat
	));

	//return std::move(s);
}




int main(int argc, char** argv)
{
	// Initialise ImageMagick library
	Magick::InitializeMagick(*argv);
	srand(4);

	if (argc < 4) {
		fmt::print("Usage: {0} outputFile.png heightInPixels numEntities [-a]\n", argv[0]);
		return 1;
	}

	std::string outputFile{ argv[1] };
	uint32_t height = std::stoi(argv[2]);
	uint32_t numEntities = std::stoi(argv[3]);
	bool acceleration = argc > 4;

	fmt::print("output: {0}\theight: {1}\tentities: {2}\tacceleration: {3}\t", outputFile, height, numEntities, acceleration);


	uint32_t width = uint32_t(height * (16./9.));


	//uint8_t(*pix)[3] = new uint8_t[height * width][3];
	PngPixel* pix = new PngPixel[height * width];

	Scene scene{ acceleration };
	initScene(scene, numEntities);
	scene.setupAccelerationStructure();

	Renderer renderer{width, height, scene};

	auto t1 = std::chrono::high_resolution_clock::now();
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			pix[width * y + x] = PngPixel{ renderer.render(x, y) * 255.f };
		}
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	fmt::print("elapsed: {} seconds\n", ms_double.count()/1000.);

	// Create Image object and read in from pixel data above
	Magick::Image image;
	image.read(width, height, "RGB", Magick::CharPixel, pix);

	// Write the image to a file - change extension if you want a GIF or JPEG
	image.write(outputFile);
	
	// open image for viewing
	system(fmt::format("start {0}", outputFile).c_str());

	return 0;
}
