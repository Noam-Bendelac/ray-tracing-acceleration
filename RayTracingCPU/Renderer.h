
#pragma once

#include "Scene.h"


#include <glm/glm.hpp>
#include <optional>
#include <memory>



struct Camera {

	glm::vec3 position;
	glm::vec3 forwardDir, upDir, rightDir;

};




struct PngPixel {
	/*uint8_t rgb[3];*/
	glm::u8vec3 rgb;
	PngPixel() : rgb() {}
	explicit PngPixel(glm::vec3 f_rgb) : rgb(glm::clamp(f_rgb, 0.f, 1.f) * 255.f) {}
};



class Renderer {
private:
	uint32_t width, height;

	Scene& scene;

	Camera camera{
		.position = {0, -16, 3},
		.forwardDir = {0, 1, 0},
		.upDir = {0, 0, 1},
		.rightDir = {1, 0, 0}
	};

public:
	Renderer(uint32_t width, uint32_t height, Scene& scene) : width(width), height(height), scene(scene) { }

	std::unique_ptr<PngPixel[]> renderFrame();



	glm::vec3 renderPixel(uint32_t x, uint32_t y) const;
	glm::vec3 renderRay(Ray const& ray, uint32_t iterDepth) const;
};


