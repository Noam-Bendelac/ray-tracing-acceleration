
#pragma once

#include <glm/glm.hpp>


// like vertex attributes/varyings
struct SurfacePoint {
	glm::vec3 pos;
	glm::vec3 norm;
	// glm::vec2 uv;
};



class Material {
public:
	virtual glm::vec3 shade(const SurfacePoint& p) = 0;
};

class DiffuseMaterial : public Material {
public:
	// conceptually like uniforms
	glm::vec3 baseColor;

	DiffuseMaterial(glm::vec3 baseColor) : baseColor(baseColor) { }

	glm::vec3 shade(const SurfacePoint& p) override;
};


