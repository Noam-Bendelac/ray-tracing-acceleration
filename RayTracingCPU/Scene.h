
#pragma once


#include "Entity.h"

#include <vector>
#include <optional>
#include <memory>



struct Hit {
	float t;
	const Entity& entity;

	auto operator<=>(const Hit& rhs) const { return t <=> rhs.t; }
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


class Scene {
public:
	std::vector<std::unique_ptr<Entity>> entities;

	Scene() {}

	OptHit raycast(Ray ray);
};


