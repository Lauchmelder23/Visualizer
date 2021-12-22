#pragma once

#include <memory>
#include "Transformable.hpp"
#include "Drawable.hpp"

// TODO: Find better name
class CameraBase : public Transformable
{
public:
	// "Scaling" doesn't really makes sense for a camera
	void GetScale() = delete;
	void SetScale(const glm::vec3&) = delete;
	void Scale(const glm::vec3&) = delete;

	inline void LookAt(const glm::vec3& target)
	{
		transformation = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	inline const glm::mat4& GetView() const
	{
		return transformation;
	}

	inline const glm::mat4& GetProjection() const
	{
		return projection;
	}

	inline void Draw(const Drawable& drawable) const
	{
		drawable.Draw(*this);
	}

protected:
	glm::mat4 projection;
};


class Camera : public CameraBase
{
public:
	Camera(float fov = 90.0f, float aspect = 1.0f, float zNear = 0.01f, float zFar = 100.0f)
	{
		projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	}

	inline void Update(float fov, float aspect, float zNear, float zFar)
	{
		projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
	}
};

class OrthogonalCamera : public CameraBase
{
public:
	OrthogonalCamera(float left = -1.0f, float right = 1.0f, float bottom = -1.0f, float top = 1.0f, float zNear = -100.0f, float zFar = 100.0f)
	{
		projection = glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	inline void Update(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		projection = glm::ortho(left, right, bottom, top, zNear, zFar);
	}
};