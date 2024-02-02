
#pragma once

#include <glm/glm.hpp>

struct Ray {
public:
	const glm::vec3 origin, direction;
	const glm::vec3 invDirection;

	Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction), invDirection(1.f / direction) { }
	glm::vec3 at(float t) const { return origin + direction * t; }
};
