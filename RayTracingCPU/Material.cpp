


#include "Material.h"

using glm::vec3;


// compromise between queries hitting the origin surface point (low epsilon causes spots of shadow where there should be light)
// or skipping an immediate intersection with another surface (high epsilon causes gaps around intersections of surfaces)
const float EPSILON = 0.001f;


vec3 DiffuseMaterial::shade(
	const SurfacePoint& p,
	const Ray& viewRay,
	std::function<glm::vec3(Ray const&)> queryColor,
	std::function<std::optional<bool>(Ray const&)> queryDepth
) {
	vec3 lightDir = glm::normalize(vec3{ -1, -1, 1 });
	vec3 dirLightIn = queryDepth(Ray(p.pos + EPSILON * lightDir, lightDir)) ? vec3{ 0 } : vec3{ 1 };
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
	vec3 lightIn = queryColor(Ray(p.pos + EPSILON * reflectDir, reflectDir));
	
	return lightIn * specularColor;
}


vec3 RefractMaterial::shade(
	const SurfacePoint& p,
	const Ray& viewRay,
	std::function<glm::vec3(Ray const&)> queryColor,
	std::function<std::optional<bool>(Ray const&)> queryDepth
) {
	// light entering the material's medium == ray leaving / direction pointing in the same direction as normal
	bool lightEntering = glm::dot(viewRay.direction, p.norm) > 0.0f;
	//vec3 refractDir = glm::refract(viewRay.direction, p.norm, lightEntering ? indexOfRefraction : 1.0f/indexOfRefraction);
	//vec3 refractDir = lightEntering ? glm::refract(viewRay.direction, p.norm * (lightEntering ? -1.f : 1.f), indexOfRefraction) : viewRay.direction;
	vec3 refractDir = lightEntering
		? glm::refract(viewRay.direction, -p.norm, indexOfRefraction)
		: glm::refract(viewRay.direction, p.norm, 1.0f / indexOfRefraction);
	if (refractDir == vec3{ 0 }) refractDir = glm::reflect(viewRay.direction, p.norm);
	vec3 lightIn = queryColor(Ray(p.pos + EPSILON * refractDir, refractDir));

	vec3 attenuationOut = lightEntering ? glm::pow(transmitColor, vec3{ glm::distance(viewRay.origin, p.pos)/10.f }) : vec3{ 1.f };

	return lightIn * attenuationOut;
}
