
#pragma once

#include "Ray.h"

#include <glm/glm.hpp>
#include <optional>


class AABB {
public:
	const glm::vec3 min, max;

	glm::vec3 mid() const { return (min + max) * 0.5f; }

	AABB() : AABB(glm::vec3(), glm::vec3()) { }
	AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) { }

	AABB& operator=(const AABB& rhs) {
		this->~AABB();
		new (this) AABB(rhs);
		return *this;
	}

	glm::vec3 size() const { return max - min; }

	// union operation
	AABB operator||(const AABB& rhs) const { return AABB {
		glm::min(min, rhs.min), glm::max(max, rhs.max)
	}; }

	// intersection operation, possibly returns invalid AABB
	/*AABB operator&&(const AABB& rhs) const { return AABB {
		glm::max(min, rhs.min), glm::min(max, rhs.max)
	}; }*/


	bool contains(const AABB& rhs) const {
		return glm::all(glm::lessThan(min, rhs.min)) && glm::all(glm::greaterThan(max, rhs.max));
	}


	std::optional<std::pair<float, float>> raycast(const Ray& r) const;



};
