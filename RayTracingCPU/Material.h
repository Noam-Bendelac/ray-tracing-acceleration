
#pragma once

#include "Ray.h"

#include <glm/glm.hpp>
#include <functional>
#include <optional>


// like vertex attributes/varyings
struct SurfacePoint {
	glm::vec3 pos;
	glm::vec3 norm;
	// glm::vec2 uv;
};



class Material {
public:
	virtual glm::vec3 shade(
		const SurfacePoint& p,
		const Ray& viewRay,
		std::function<glm::vec3(Ray const&)> queryColor,
		std::function<std::optional<bool>(Ray const&)> queryDepth
	) = 0;
};

class DiffuseMaterial : public Material {
public:
	// conceptually like uniforms
	glm::vec3 baseColor;

	DiffuseMaterial(glm::vec3 baseColor) : baseColor(baseColor) { }

	glm::vec3 shade(
		const SurfacePoint& p,
		const Ray& viewRay,
		std::function<glm::vec3(Ray const&)> queryColor,
		std::function<std::optional<bool>(Ray const&)> queryDepth
	) override;
};

class ReflectMaterial : public Material {
public:
	glm::vec3 specularColor;

	ReflectMaterial(glm::vec3 specularColor) : specularColor(specularColor) { }

	glm::vec3 shade(
		const SurfacePoint& p,
		const Ray& viewRay,
		std::function<glm::vec3(Ray const&)> queryColor,
		std::function<std::optional<bool>(Ray const&)> queryDepth
	) override;
};

class RefractMaterial : public Material {
public:
	glm::vec3 transmitColor;
	float indexOfRefraction;

	RefractMaterial(glm::vec3 transmitColor, float ior) : transmitColor(transmitColor), indexOfRefraction(ior) { }

	glm::vec3 shade(
		const SurfacePoint& p,
		const Ray& viewRay,
		std::function<glm::vec3(Ray const&)> queryColor,
		std::function<std::optional<bool>(Ray const&)> queryDepth
	) override;
};


