


#include "Material.h"

using glm::vec3;

vec3 DiffuseMaterial::shade(const SurfacePoint& p) {
	vec3 lightDir = glm::normalize(vec3{ -1, -1, 1 });
	float lambert = glm::clamp(glm::dot(p.norm, lightDir), 0.f, 1.f);
	return baseColor * lambert;
}
