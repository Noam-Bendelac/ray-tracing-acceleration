
#pragma once


#include "AABB.h"

#include <memory>
#include <vector>
#include <concepts>
#include <functional>
#include <array>
#include <optional>
#include <utility>
#include <glm/gtx/component_wise.hpp>



// for some reason, implementing these methods in a OctTree.cpp causes linking errors.

// OctTree<T> owns Ts. if you want OctTree not to own your entities, you may use reference_wrapper or shared_ptr
template <typename T, typename AABBAccessor>
// TODO should i add a requires on AABBAccessor?
//requires requires (AABBAccessor a, const T& t) {
//	{a(t)} -> std::same_as<const AABB&>;
//}
class OctTree {
public:

	inline static AABBAccessor aabbAccessor{};
	//std::function<const AABB& (T)> aabbAccessor;


	void insert(T&& t) { root.insert(std::move(t)); }

	void raycastForEach(const Ray& ray, const std::function<void(const T&)>& f) {
		auto forEachT = [&ray, &f](const T& t) {
			if (aabbAccessor(t).raycast(ray)) {
				f(t);
			}
		};
		std::function<void(const Node& n)> forEachN = [&ray, &f, &forEachT, &forEachN](const Node& n) {
			if (n.aabb.raycast(ray)) {
				n.traverse(forEachT, forEachN);
			}
		};
		root.traverse(forEachT, forEachN);

	}

	OctTree(AABB aabb) : root(aabb) { }
	OctTree(OctTree&& o) : root(std::move(o.root)) { }

private:
	static const uint32_t BUCKET_SIZE = 4;

	// for some reason, implementing Node's methods in a OctTree.cpp causes linking errors.
	class Node {
public:

	// keeps track of all entities
	// if leaf: equals number of initialized entities in bucket
	// if internal: equals sum of children's dataCounts + boundaryData.size
	uint32_t dataCount = 0;
	bool leafNode() const { return dataCount <= BUCKET_SIZE; }
	union Data {
		// leaf case
		// using optional<T> because array must initilize all members despite dataCount=0 but T might not have a default constructor
		// is there a better way to do this? like a vector but with no heap allocation, i.e. takes capacity in the template
		std::array<std::optional<T>, BUCKET_SIZE> bucket;

		// internal node case
		struct {
			// stores only entites that fit neatly into exactly one child
			std::unique_ptr<std::array<Node, uint8_t(8)>> children;
			// entities that straddle two or more children, thus shouldn't be placed into any one child
			std::vector<T> boundaryData;
		};

		// construction and destruction handled by parent class because the union itself does not know what variant it's in
		Data() : bucket() {}
		~Data() {}
	} data;

	AABB aabb;


	Node() : Node(AABB()) { }

	Node(AABB aabb) : aabb(aabb), dataCount(0), data() {
		// data.bucket might already be initialized to nullopts by the Data() constructor
		//new (&data.bucket) std::array<std::optional<T>, BUCKET_SIZE>();
	}

	Node(Node&& n) : aabb(n.aabb), dataCount(n.dataCount) {
		if (n.leafNode()) {
			new (&data.bucket) std::array<std::optional<T>, BUCKET_SIZE>(std::move(n.data.bucket));
		}
		else {
			data.boundaryData = std::move(n.data.boundaryData);
			data.children = std::move(n.data.children);
		}
	}

	~Node() {
		// check tag for appropriate destructor
		// could be good to encapsulate all this logic in a struct wrapper around the union
		if (leafNode()) {
			data.bucket.~array<std::optional<T>, BUCKET_SIZE>();
		}
		else {
			data.children.~unique_ptr<std::array<Node, uint8_t(8)>>();
			data.boundaryData.~vector<T>();
		}
	}

	void insert(T&& t) {
		if (leafNode()) {
			if (dataCount < BUCKET_SIZE) {
				// still room to insert
				data.bucket[dataCount++] = t;
			}
			else {
				// must transform into internal node
				// save bucket contents
				std::array<std::optional<T>, BUCKET_SIZE> bucket{ std::move(data.bucket) };
				// is destructor necessary/correct after moving out? c++ move is nondestructive so i think this is ok?
				data.bucket.~array<std::optional<T>, BUCKET_SIZE>();

				// init internal node fields - this overwrites data.bucket!
				// reassignment might call destructors on the garbage data in children and boundaryData
				// so i use placement new instead
				new (&data.children) std::unique_ptr<std::array<Node, 8>>(new std::array<Node, 8>());
				new (&data.boundaryData) std::vector<T>();

				// set bounds on 8 children
				for (size_t i = 0; i < (*data.children).size(); ++i) {
					// use "bitvector" i to select location of each of 8 cells
					glm::bvec3 which{
						i & (1 << 0),
						i & (1 << 1),
						i & (1 << 2),
					};
					// is (*data.children)[i] already initialized? i thought not, but intellisense says
					// doing ...[i] = Node{...} calls a deleted destructor
					// this is because array<Node> calls Node's default constructor
					(*data.children)[i].aabb = AABB{
						// select between min-mid or mid-max for each axis
						glm::mix(aabb.min, aabb.mid(), which),
						glm::mix(aabb.mid(), aabb.max, which),
					};
				}

				// move from old bucket to new fields depending on boundary overlaps
				for (size_t i = 0; i < dataCount; ++i) {
					if (auto& tb = bucket[i]) {
						insertInternal(std::move(tb.value()));
					};
				}
				// move new element t to new fields
				insertInternal(std::move(t));
				dataCount++;
			}
		}
		else {
			// internal node
			insertInternal(std::move(t));
			dataCount++;
		}
	}

	/**
	* @pre this is an internal node with initialized children and boundaryData
	*/
	void insertInternal(T&& t) {
		// AABBAccessor must have an operator() that takes const T& and returns const AABB&

		glm::bvec3 minLess = glm::lessThan(aabbAccessor(t).min, aabb.mid());
		glm::bvec3 maxGrtr = glm::greaterThan(aabbAccessor(t).max, aabb.mid());
		if (glm::any(minLess && maxGrtr)) {
			// if there are any axes on which t's min and max straddle either side of our node's mid,
			// then t does not fit into any single child, so keep it in boundaryData
			data.boundaryData.emplace_back(std::move(t));
		}
		else {
			// there are no axes on which t's min is less than our mid and t's max is greater than our mid
			// so on all axes, either min and max are both less than mid or both greater than mid

			// bitwise components of child index based on each axis
			glm::uvec3 axisIndices = glm::uvec3{
				1 << 0,
				1 << 1,
				1 << 2,
			};
			// we can read maxGrtr on each axis to find which half it should be on
			uint8_t i = glm::compAdd(glm::mix(glm::uvec3{ 0 }, axisIndices, maxGrtr));
			(*data.children)[i].insert(std::move(t));
		}
	}


	void traverse(std::function<void(const T&)> forEachT, std::function<void(const Node&)> forEachN) const {
		if (leafNode()) {
			for (size_t i = 0; i < dataCount; ++i) {
				if (const auto& t = data.bucket[i]) {
					forEachT(t.value());
				}
			}
		}
		else {
			for (const T& t : data.boundaryData) {
				forEachT(t);
			}
			for (const Node& n : *data.children) {
				forEachN(n);
			}
		}
	}

	};


	Node root;


};

