

#include "AABB.h"

#include <algorithm>
#include <limits>

using glm::vec3;

std::optional<std::pair<float, float>> AABB::raycast(const Ray& r) const {
	// on each axis, do we reach the min bound first or the max bound first? (first means smaller t value).
	// this depends on the sign of r.direction in that axis.
	// we want to organize the intervals on x, y, and z based on which one the ray hits first (lesser in t value),
	// not which one is lesser in world space
	glm::bvec3 dirPositive = glm::greaterThan(r.direction, vec3{ 0 });
	// mix(ifFalse, ifTrue, boolVec)
	vec3 xyzFirst = glm::mix(max, min, dirPositive);
	vec3 xyzLast = glm::mix(min, max, dirPositive);


	// xyz = ro + t*rd
	// (xyz - ro)/rd = t
	// component wise division gives the t of intersection for each of the 3 axes
	// [xyzFirst, xyzLast] form the interval of intersection for each axis separately
	// when the ray enters the region at xyzFirst, that's the smaller of the t values
	// when the ray leaves at xyzLast, that's the bigger of the t values
	// we also want to bypass division by zero for any axis with dir=0 and set that axis's range to -inf, inf
	vec3 inf = vec3{ std::numeric_limits<float>::infinity() };
	vec3 minTs = glm::mix((xyzFirst - r.origin) / r.direction, -inf, glm::equal(r.direction, vec3{ 0 }));
	vec3 maxTs = glm::mix((xyzLast - r.origin) / r.direction, inf, glm::equal(r.direction, vec3{ 0 }));

	// we want to find the range where all 3 axes are within their t range
	// the overlap is from the biggest of the mins to the smallest of the maxes
	float minT = std::max(std::max(minTs.x, minTs.y), minTs.z);
	float maxT = std::min(std::min(maxTs.x, maxTs.y), maxTs.z);

	// only overlaps if interval is valid
	if (minT < maxT) {
		return { { minT, maxT } };
	}
	else {
		return std::nullopt;
	}
}
