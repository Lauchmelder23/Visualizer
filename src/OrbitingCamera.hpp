#pragma once

#include <lol/lol.hpp>

class OrbitingCamera : public lol::Camera
{
public:
	OrbitingCamera() {}
	OrbitingCamera(const glm::vec3& target, float distance, const glm::vec2& verticalRange = glm::vec2(1.0f, 179.0f));

	inline void SetPosition(float pitch, float yaw, float distance)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->distance = distance;

		CalculateMatrix();
	}

	inline glm::vec2 GetAngles()
	{
		return glm::vec2(pitch, yaw);
	}

	void Pan(float amount);
	void Tilt(float amount);
	void Zoom(float amount);

private:
	void CalculateMatrix();

private:
	float pitch, yaw;
	float distance;
	glm::vec3 target;

	glm::vec2 pitchRange;
};