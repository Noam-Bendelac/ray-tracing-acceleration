



#include "Renderer.h"
#include "Entity.h"

#include <glm/gtx/norm.hpp>


using glm::vec3;
using glm::vec2;









vec3 Renderer::render(uint32_t x, uint32_t y) {
	// center each ray origin inside the pixel, so add 0.5
	// height-y so -1 is at the bottom
	vec2 ndc(vec2{ x + 0.5, height - (y + 0.5) } / vec2{ width, height } * 2.f - vec2{ 1 });

	float aspect = float(width) / height;
	// inverted sensor position like in a real camera
	vec2 sensorPos2d = -ndc * vec2{ aspect, 1 };
	float focalLen = 1.8f;
	vec3 sensorPos = camera.position - camera.forwardDir * focalLen + glm::mat2x3{ camera.rightDir, camera.upDir } * sensorPos2d;

	// ray origin and direction
	vec3 ro = camera.position;
	vec3 rd = normalize(ro - sensorPos);
	
	auto ray = Ray(ro, rd);


	return renderRay(ray, 0);
}


vec3 Renderer::renderRay(Ray const& ray, uint32_t iterDepth) {
	if (iterDepth > 7) {
		return vec3{ 0.1f };
	}
	auto hit = scene.raycast(ray);
	return hit
		.transform([&ray, this, iterDepth](Hit hit) {
			return hit.entity.material->shade(
				hit.entity.surface(ray.at(hit.t)),
				ray,
				[this, iterDepth](Ray const& ray2) { return renderRay(ray2, iterDepth + 1); },
				[this](Ray const& ray2) { return scene.raycast(ray2).transform([](Hit const& hit) { return hit.t; }); }
			);
		})
		.value_or(vec3{ 0.1f });
}









