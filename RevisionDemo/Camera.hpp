#pragma once
#include "superHeader.hpp"

class Camera {
private:
	mat4 projection;
	mat4 view;
	mat4 model;
	vec3 position;
	vec3 lookAtPosition;
	vec3 whatIsUp;
public:
	Camera(float fov, float aspect, float zNear, float zFar);
	vec3 setCameraPosition(vec3 setpos);
	vec3 setLookAtPosition(vec3 lookat);
	vec3 getCameraPosition();
	vec3 getLookAtPosition();
	mat4 getMVP();
};