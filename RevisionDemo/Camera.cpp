#include "Camera.hpp"
Camera::Camera(float fov, float aspect, float zNear, float zFar) {
	this->projection = perspective(fov, aspect, zNear, zFar);
}
vec3 Camera::setCameraPosition(vec3 setpos) {
	this->position = setpos;
	return this->position;
}
vec3 Camera::setLookAtPosition(vec3 lookat) {
	this->lookAtPosition = lookat;
	return this->lookAtPosition;
}
vec3 Camera::getCameraPosition() {
	return this->position;
}
vec3 Camera::getLookAtPosition() {
	return this->lookAtPosition;
}
mat4 Camera::getMVP() {
	this->view = glm::lookAt(
		this->position,
		this->lookAtPosition,
		vec3(0.0f, 1.0f, 0.0)
		);
	this->model = mat4(1.f);
	return this->projection * this->view * this->model;
}