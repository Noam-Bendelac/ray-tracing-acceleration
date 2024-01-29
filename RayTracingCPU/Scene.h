
#pragma once


#include "Entity.h"
#include "OctTree.h"

#include <vector>
#include <optional>
#include <memory>
#include <functional>



struct Hit {
	float t;
	const Entity& entity;

	auto operator<=>(const Hit& rhs) const { return t <=> rhs.t; }

	Hit& operator=(const Hit& rhs) {
		this->~Hit();
		new (this) Hit(rhs);
		return *this;
	}
};

struct OptHit : public std::optional<Hit> {
	//OptHit(std::optional<Hit>&& oh) : std::optional<Hit>(oh) {}

	// get hit's t, but no hit returns infinity such that Some(t) < None
	float t() const {
		return transform([](Hit h) { return h.t; })
			.value_or(std::numeric_limits<float>::infinity());
	}

	auto operator<=>(const OptHit& rhs) const { return t() <=> rhs.t(); }
};



struct AABBAccessor {
	const AABB& operator()(const std::reference_wrapper<const Entity>& rwe) {
		return rwe.get().aabb();
	}
};


class Scene {
public:
	std::vector<std::unique_ptr<Entity>> entities;

	Scene(bool acceleration) : entities(), acceleration(acceleration) { }
	Scene(Scene&& s) : entities(std::move(s.entities)), tree(std::move(s.tree)), acceleration(s.acceleration) {}

	void setupAccelerationStructure();

	OptHit raycast(const Ray& ray);

private:
	bool acceleration;
	OctTree<std::reference_wrapper<const Entity>, AABBAccessor> tree{ AABB{glm::vec3{-20,-20,-20}, glm::vec3{20,20,20}} };
};


