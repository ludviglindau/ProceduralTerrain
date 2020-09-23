#pragma once
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

using namespace glm;

class Camera {
	vec3 pos;
	vec3 dir;
	mat4 view;
	mat4 proj;
public:
	Camera(vec3 pos = vec3(0.0), vec3 facing = vec3(0.0, 0.0, -1.0), float fov = 75.f) {
		proj = perspective(radians(fov), 16.f / 9.f, 0.5f, 10000.0f);
		this->pos = pos;
		this->dir = normalize(facing);
		orientation.y = dir.y * 3.14159f;
		orientation.x = (cos(dir.x) + sin(dir.y)) * 3.14159f;
		view = glm::mat4(1.0);
	}

	void relativeMove(vec3 move) { pos += vec3(inverse(getView()) * vec4(move, 0.0)); }
	mat4 getViewProj() { view = getView(); return proj * view; }
	mat4 getView() { 
		view = glm::mat4(1.0);
		view =
			rotate(view, orientation.y, glm::vec3(1.f, 0.f, 0.f)) *
			rotate(view, orientation.x, glm::vec3(0.f, 1.f, 0.f)) *
			translate(view, -pos);
		return view;
	}
	mat4 getProjection() { return proj; }
	vec3 getPos() { return pos; }

	float sensitivity = 0.01f;
	vec2 orientation = vec2(0.0);
};
