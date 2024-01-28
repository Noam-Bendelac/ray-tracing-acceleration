
#pragma once

#include <glm/glm.hpp>

struct Ray {
public:
	glm::vec3 origin, direction;
	glm::vec3 at(float t) const { return origin + direction * t; }
};
