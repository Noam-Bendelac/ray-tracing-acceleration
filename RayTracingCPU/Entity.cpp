


#include "Entity.h"


#include <glm/gtx/norm.hpp>


using glm::vec3;



std::optional<float> Sphere::raycast(Ray r) const {
	auto ro = r.origin, rd = r.direction;

	// raycast
	float a = 1;
	float b = 2 * dot(ro - center, rd);
	float c = length2(ro - center) - radius * radius;

	float disc = b * b - 4 * a * c;
	if (disc < 0) {
		return std::nullopt;
	}
	else {
		float t1 = (-b - sqrt(disc)) / (2 * a);
		//vec3 p = ro + t1 * rd;

		return t1;
	}
}


SurfacePoint Sphere::surface(vec3 pos) const {
	vec3 norm = glm::normalize(pos - center);
	return SurfacePoint{
		.pos = pos,
		.norm = norm,
	};
}

