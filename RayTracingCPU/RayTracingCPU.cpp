// RayTracingCPU.cpp : Defines the entry point for the application.
//



#include "RayTracingCPU.h"
#include "Renderer.h"

#include <fmt/core.h>
#include <Magick++.h>
#include <glm/glm.hpp>
#include <memory>
#include <cstdlib>



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

void initScene(Scene& s) {
	auto mat = new DiffuseMaterial{ glm::vec3{1,1,0} };
	
	for (int i = 0; i < 700; ++i) {
		s.entities.push_back(std::make_unique<Sphere>(
			glm::vec3{ randF() * 16. - 8., randF() * 16. - 8., randF() * 16. - 8. },
			0.5f,
			*mat
		));
	}

	s.entities.push_back(std::make_unique<Sphere>(
		glm::vec3{ 0, 0, -100 },
		100.f,
		*mat
	));

	//return std::move(s);
}




int main(int argc, char** argv)
{
	// Initialise ImageMagick library
	Magick::InitializeMagick(*argv);

	fmt::print("Hello\n");

	srand(4);

	uint32_t height = 480;
	uint32_t width = uint32_t(height * (16./9.));


	//uint8_t(*pix)[3] = new uint8_t[height * width][3];
	PngPixel* pix = new PngPixel[height * width];

	Scene scene;
	initScene(scene);
	scene.setupAccelerationStructure();

	Renderer renderer{width, height, scene};

	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			pix[width * y + x] = PngPixel{ renderer.render(x, y) * 255.f };
		}
	}

	// Create Image object and read in from pixel data above
	Magick::Image image;
	image.read(width, height, "RGB", Magick::CharPixel, pix);

	// Write the image to a file - change extension if you want a GIF or JPEG
	image.write("result.png");
	
	// open image for viewing
	system("start result.png");

	return 0;
}
