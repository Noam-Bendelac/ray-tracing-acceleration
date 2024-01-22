



#include "Renderer.h"

#include <glm/gtx/norm.hpp>


using glm::vec3;
using glm::vec2;
using glm::normalize;
using glm::length2;
using glm::dot;
using glm::clamp;



class Material;


struct Hit {
	float t;
	// conceptually like vertex attributes
	vec3 pos;
	vec3 normal;
	//vec2 uv;
	Material& mat;
};




class Material {
public:
	virtual vec3 shade(Hit hit) = 0;
};

class DiffuseMaterial : public Material {
public:
	// conceptually like uniforms
	vec3 baseColor;

	DiffuseMaterial(vec3 baseColor) : baseColor(baseColor) { }

	vec3 shade(Hit hit) {
		vec3 lightDir = normalize(vec3{ 1, -1, 1 });
		float lambert = clamp(dot(hit.normal, lightDir), 0.f, 1.f);
		return baseColor * lambert;
	}
};





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

	auto hit = raycast(ro, rd);
	return hit
		.transform([](Hit hit) { return hit.mat.shade(hit); })
		.value_or(vec3{ 0, 0, 0 });

}





DiffuseMaterial mat{ vec3{1,1,0} };


std::optional<Hit> Renderer::raycast(vec3 origin, vec3 direction) {
	auto ro = origin, rd = direction;

	// raycast
	vec3 center = { 0, 0, 0 };
	float radius = 1;
	float a = 1;
	float b = 2 * dot(ro - center, rd);
	float c = length2(ro - center) - radius * radius;

	float disc = b * b - 4 * a * c;
	if (disc < 0) {
		return std::nullopt;
	}
	else {
		float t1 = (-b - sqrt(disc)) / (2 * a);
		vec3 p = ro + t1 * rd;
		vec3 norm = normalize(p - center);


		//return { 255. * x / width, 255. * y / height, 0};
		return Hit {
			.t = t1,
			.pos = p,
			.normal = norm,
			.mat = mat
		};
	}
}




