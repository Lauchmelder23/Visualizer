#include "Transformable.hpp"

Transformable::Transformable() :
	position(0.0f), scale(1.0f), orientation(0.0, 0.0, 0.0, 1.0)
{
	CalculateTransformationMatrix();
}

const glm::vec3& Transformable::GetPosition()
{
	return position;
}

void Transformable::SetPosition(const glm::vec3& pos)
{
	position = pos;
	CalculateTransformationMatrix();
}

void Transformable::Move(const glm::vec3& direction)
{
	position += direction;
	CalculateTransformationMatrix();
}

const glm::vec3 Transformable::GetRotation()
{
	return glm::eulerAngles(orientation);
}

void Transformable::SetRotation(const glm::vec3& axis, float angle)
{
	orientation = glm::quat(glm::radians(angle), axis);
	CalculateTransformationMatrix();
}

void Transformable::SetRotation(const glm::vec3& eulerAngles)
{
	orientation = glm::quat(eulerAngles);
	CalculateTransformationMatrix();
}

void Transformable::Rotate(const glm::vec3& axis, float angle)
{
	orientation = glm::rotate(orientation, glm::radians(angle), axis);
	CalculateTransformationMatrix();
}

const glm::vec3& Transformable::SetScale()
{
	return scale;
}

void Transformable::SetScale(const glm::vec3& scale)
{
	this->scale = scale;
	CalculateTransformationMatrix();
}

void Transformable::Scale(const glm::vec3& factor)
{
	this->scale *= scale;	// I pray this is component-wise multiplication
	CalculateTransformationMatrix();
}

void Transformable::CalculateTransformationMatrix()
{
	transformation = glm::mat4(1.0f);
	glm::scale(transformation, scale);
	transformation *= glm::toMat4(orientation);
	glm::translate(transformation, position);
}
