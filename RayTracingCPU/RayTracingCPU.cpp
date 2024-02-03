// RayTracingCPU.cpp : Defines the entry point for the application.
//



#include "RayTracingCPU.h"
#include "Renderer.h"

#include <fmt/core.h>
#include <Magick++.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
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



float randF() {
	return float(rand()) / RAND_MAX;
}


std::shared_ptr<Material> randMaterial() {
	auto r = rand() % 3;
	if (r == 0) {
		return std::make_shared<DiffuseMaterial>(glm::abs(glm::sphericalRand(1.f)));
	}
	else if (r == 1) {
		return std::make_shared<ReflectMaterial>(glm::abs(glm::sphericalRand(1.f)));
	}
	else {
		return std::make_shared<RefractMaterial>(glm::abs(glm::sphericalRand(1.f)), 2.0f);
	}
}

void initScene(Scene& s, uint32_t numEntities) {
	// TODO scene cleanup function, maybe destructor, fix leak

	//auto mat = std::make_shared<DiffuseMaterial>(glm::abs(glm::sphericalRand(1.f)));
	
	for (uint32_t i = 0; i < numEntities; ++i) {
		s.entities.push_back(std::make_unique<Sphere>(
			glm::vec3{ randF() * 16. - 8., randF() * 20. - 10., randF() * 10. - 2. },
			4.f/powf(float(numEntities), 1.0f/3),
			randMaterial()
		));
	}

	glm::vec3 floorColor = glm::abs(glm::sphericalRand(1.f));
	floorColor = glm::vec3{ floorColor.b, floorColor.g, floorColor.r };
	s.entities.push_back(std::make_unique<Sphere>(
		glm::vec3{ 0, 0, -200 },
		200.f,
		std::make_shared<DiffuseMaterial>(floorColor)
		//std::make_shared<ReflectMaterial>(glm::vec3{ 1 })
		//std::make_shared<ReflectMaterial>(glm::abs(glm::sphericalRand(1.f)))
	));

	//return std::move(s);
}




int main(int argc, char** argv)
{
	// Initialise ImageMagick library
	Magick::InitializeMagick(*argv);
	srand(5);

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



	Scene scene{ acceleration };
	initScene(scene, numEntities);
	scene.setupAccelerationStructure();

	Renderer renderer{width, height, scene};

	auto t1 = std::chrono::high_resolution_clock::now();
	auto pix = renderer.renderFrame();
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	fmt::print("elapsed: {} seconds\n", ms_double.count() / 1000.);

	// Create Image object and read in from pixel data above
	Magick::Image image;
	image.read(width, height, "RGB", Magick::CharPixel, pix.get());

	// Write the image to a file - change extension if you want a GIF or JPEG
	image.write(outputFile);
	
	// open image for viewing
	system(fmt::format("start {0}", outputFile).c_str());

	return 0;
}
