


#include "Material.h"

using glm::vec3;


const float EPSILON = 0.001f;


vec3 DiffuseMaterial::shade(
	const SurfacePoint& p,
	const Ray& viewRay,
	std::function<glm::vec3(Ray const&)> queryColor,
	std::function<std::optional<bool>(Ray const&)> queryDepth
) {
	vec3 lightDir = glm::normalize(vec3{ -1, -1, 1 });
	vec3 dirLightIn = queryDepth(Ray{ .origin = p.pos + EPSILON * lightDir, .direction = lightDir }) ? vec3{ 0 } : vec3{ 1 };
	float lambert = glm::clamp(glm::dot(p.norm, lightDir), 0.f, 1.f);
	vec3 ambient = vec3{ 0.1f };
	return (dirLightIn * lambert + ambient) * baseColor;
}


vec3 ReflectMaterial::shade(
	const SurfacePoint& p,
	const Ray& viewRay,
	std::function<glm::vec3(Ray const&)> queryColor,
	std::function<std::optional<bool>(Ray const&)> queryDepth
) {
	vec3 reflectDir = glm::reflect(viewRay.direction, p.norm);
	vec3 lightIn = queryColor(Ray{ .origin = p.pos + EPSILON * reflectDir, .direction = reflectDir });
	
	return lightIn * specularColor;
}
