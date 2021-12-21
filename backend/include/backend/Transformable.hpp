#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Transformable
{
public:
	Transformable();

	const glm::vec3& GetPosition() const;
	void SetPosition(const glm::vec3& pos);
	void Move(const glm::vec3& direction);

	const glm::vec3 GetRotation() const;
	const glm::quat& GetQuaternion() const;
	void SetRotation(const glm::vec3& axis, float angle);
	void SetRotation(const glm::vec3& eulerAngles);
	void Rotate(const glm::vec3& axis, float angle);

	const glm::vec3& GetScale() const;
	void SetScale(const glm::vec3& scale);
	void Scale(const glm::vec3& factor);

private:
	void CalculateTransformationMatrix();

protected:
	glm::mat4 transformation;

	glm::vec3 position, scale;
	glm::quat orientation;
};