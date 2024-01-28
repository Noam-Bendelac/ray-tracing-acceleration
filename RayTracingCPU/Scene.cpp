
#include "Scene.h"

#include <glm/glm.hpp>
#include <functional>
#include <algorithm>


using glm::vec3;





void Scene::setupAccelerationStructure() {
	for (const std::unique_ptr<Entity>& pe : entities) {
		const Entity& e = *pe;
		tree.insert(std::move(std::reference_wrapper<const Entity>(e)));
		//tree.insert(e);
	}
}



template<typename From, typename FromIter, typename To>
class MappedIter {
	using F = std::function<To(From)>;
	F f;
	FromIter iter;
public:
	// TODO copy constructor? needs to copy iter
	MappedIter(FromIter iter, F f): f(f), iter(iter) { }
	To operator*() { return f(*iter); }
	To* operator->() { return &f(*iter); }
	auto& operator++() { iter++; return *this; }
	bool operator!=(const MappedIter<From, FromIter, To>& rhs) const { return iter != rhs.iter; }
	bool operator==(const MappedIter<From, FromIter, To>& rhs) const { return iter == rhs.iter; }

	static auto makeRange(FromIter begin, FromIter end, F f) {
		return std::pair {
			MappedIter<From, FromIter, To>(begin, f),
			MappedIter<From, FromIter, To>(end, f),
		};
	}

	// TODO
	using iterator_category = std::forward_iterator_tag; // input_iterator? (i want operator* to be called only once)
	using difference_type = std::ptrdiff_t; // TODO
	using value_type = To;
	using pointer = value_type*;
	using reference = value_type&;
};






OptHit Scene::raycast(const Ray& ray) {

	OptHit nearest{ std::nullopt };

	tree.raycastForEach(ray, [&ray, &nearest](auto e) {
		OptHit hit{ e.get().raycast(ray).transform([&e](float t) { return Hit{.t = t, .entity = e.get()}; })};
		if (hit < nearest) {
			nearest = hit;
		}
	});

	return nearest;
}
