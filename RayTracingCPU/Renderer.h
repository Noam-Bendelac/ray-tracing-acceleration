
#pragma once

#include "Scene.h"


#include <glm/glm.hpp>
#include <optional>



struct Camera {

	glm::vec3 position;
	glm::vec3 forwardDir, upDir, rightDir;

	//friend Camera lookAt(vec3 targetPos, )
};


struct Hit;


class Renderer {
private:
	uint32_t width, height;

	Scene& scene;

	Camera camera{
		.position = {0, -15, 3},
		.forwardDir = {0, 1, 0},
		.upDir = {0, 0, 1},
		.rightDir = {1, 0, 0}
	};

public:
	Renderer(uint32_t width, uint32_t height, Scene& scene) : width(width), height(height), scene(scene) { }

	glm::vec3 render(uint32_t x, uint32_t y);


};


