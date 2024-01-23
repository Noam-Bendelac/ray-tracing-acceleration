



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
	vec3 sensorPos = camera.position - camera.forwardDir + glm::mat2x3{ camera.rightDir, camera.upDir } * sensorPos2d;

	// ray origin and direction
	vec3 ro = camera.position;
	vec3 rd = normalize(ro - sensorPos);

	auto ray = Ray{ .origin = ro, .direction = rd };


	auto hit = scene.raycast(ray);
	return hit.transform([&ray](Hit hit) {
		return hit.entity.material.shade(
			hit.entity.surface(ray.at(hit.t))
		); 
	}).value_or(vec3{ 0, 0, 0 });
}











