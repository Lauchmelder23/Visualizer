#include "OrbitingCamera.hpp"

OrbitingCamera::OrbitingCamera(const glm::vec3& target, float distance, const glm::vec2& pitchRange) :
	target(target), distance(distance), pitchRange(pitchRange)
{
	pitch = pitchRange.x + (pitchRange.y - pitchRange.x) / 2;
	yaw = 0.0f;
}

void OrbitingCamera::Pan(float amount)
{
	yaw += amount;
	if (yaw < 0.0f || yaw >= 360.0f)
		yaw = 0.0f;

	CalculateMatrix();
}

void OrbitingCamera::Tilt(float amount)
{
	pitch += amount;
	if (pitch < pitchRange.x)
		pitch = pitchRange.x;
	else if (pitch > pitchRange.y)
		pitch = pitchRange.y;

	CalculateMatrix();
}

void OrbitingCamera::Zoom(float amount)
{
	distance += amount;
	if (amount <= 0.0f)
		distance = amount;
}

void OrbitingCamera::CalculateMatrix()
{
	glm::vec3 position = distance * glm::vec3(
		cos(glm::radians(yaw)) * sin(glm::radians(pitch)),
		cos(glm::radians(pitch)),
		sin(glm::radians(yaw)) * sin(glm::radians(pitch))
	);

	transformation = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}
