
#pragma once

#include "Material.h"
#include "Ray.h"
#include "AABB.h"


#include <optional>
#include <memory>


class Entity {
public:
	std::shared_ptr<Material> material;
	Entity(std::shared_ptr<Material> m) : material(std::move(m)) {}
	virtual std::optional<float> raycast(const Ray& r) const = 0;
	virtual SurfacePoint surface(glm::vec3 pos) const = 0;
	virtual const AABB& aabb() const = 0;
};


class Sphere : public Entity {
	glm::vec3 center;
	float radius;
	AABB mAabb;
	AABB calcAABB() const;
public:
	Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> m) : center(center), radius(radius), Entity(std::move(m)), mAabb(calcAABB()) {}
	std::optional<float> raycast(const Ray& r) const override;
	SurfacePoint surface(glm::vec3 pos) const override;
	const AABB& aabb() const override;
};

