#pragma once

#include "backend/Camera.hpp"

class OrbitingCamera : public Camera
{
public:
	OrbitingCamera() {}
	OrbitingCamera(const glm::vec3& target, float distance, const glm::vec2& verticalRange = glm::vec2(1.0f, 179.0f));

	inline void SetPosition(float pitch, float yaw)
	{
		this->pitch = pitch;
		this->yaw = yaw;

		CalculateMatrix();
	}

	inline glm::vec2 GetAngles()
	{
		return glm::vec2(pitch, yaw);
	}

	void Pan(float amount);
	void Tilt(float amount);

private:
	void CalculateMatrix();

private:
	float pitch, yaw;
	float distance;
	glm::vec3 target;

	glm::vec2 pitchRange;
};