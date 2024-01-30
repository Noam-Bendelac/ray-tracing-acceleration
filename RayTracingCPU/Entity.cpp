


#include "Entity.h"


#include <glm/gtx/norm.hpp>


using glm::vec3;



std::optional<float> Sphere::raycast(const Ray& r) const {
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
		float sqrtdisc = sqrt(disc);
		float t1 = (-b - sqrtdisc) / (2 * a);
		// filter out negative t's (wrong side of ray)
		if (t1 > 0.0) {
			return t1;
		}
		// this allows backface intersection if ray starts inside sphere
		float t2 = (-b + sqrtdisc) / (2 * a);
		if (t2 > 0.0) {
			return t2;
		}
		return std::nullopt;
	}
}


SurfacePoint Sphere::surface(vec3 pos) const {
	vec3 norm = glm::normalize(pos - center);
	return SurfacePoint{
		.pos = pos,
		.norm = norm,
	};
}



AABB Sphere::calcAABB() const {
	return AABB{ center - vec3{radius}, center + vec3{radius} };
}

const AABB& Sphere::aabb() const {
	//return AABB{ center - vec3{radius}, center + vec3{radius} };
	return mAabb;
}
