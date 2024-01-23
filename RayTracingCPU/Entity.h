
#pragma once

#include "Material.h"


#include <optional>


struct Ray {
public:
	glm::vec3 origin, direction;
	glm::vec3 at(float t) const { return origin + direction * t; }
};

class Entity {
public:
	Material& material;
	Entity(Material& m) : material(m) {}
	virtual std::optional<float> raycast(Ray r) const = 0;
	virtual SurfacePoint surface(glm::vec3 pos) const = 0;
};


class Sphere : public Entity {
	glm::vec3 center;
	float radius;
public:
	Sphere(glm::vec3 center, float radius, Material& m) : center(center), radius(radius), Entity(m) {}
	std::optional<float> raycast(Ray r) const override;
	SurfacePoint surface(glm::vec3 pos) const override;
};

