
#include "Scene.h"

#include <glm/glm.hpp>
#include <functional>
#include <algorithm>


using glm::vec3;






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





OptHit Scene::raycast(Ray ray) {
	
	auto [hitsBegin, hitsEnd] = MappedIter<const std::unique_ptr<Entity>&, std::vector<std::unique_ptr<Entity>>::iterator, OptHit>::makeRange(
		entities.begin(),
		entities.end(),
		[&ray](const std::unique_ptr<Entity>& e) { return OptHit{
			e->raycast(ray)
			.transform([&e](float t) { return Hit {.t = t, .entity = *e }; })
		}; }
	);

	auto nearest = std::min_element(hitsBegin, hitsEnd);
	if (nearest == hitsEnd) {
		// no hits
		return OptHit{ std::nullopt };
	}
	else {
		return *nearest;
	}
}
