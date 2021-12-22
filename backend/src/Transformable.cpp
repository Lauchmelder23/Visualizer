#include "backend/Transformable.hpp"

Transformable::Transformable() :
	position(0.0f), scale(1.0f), orientation(glm::vec3(0.0, 0.0, 0.0))
{
	CalculateTransformationMatrix();
}

const glm::vec3& Transformable::GetPosition() const
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

const glm::vec3 Transformable::GetRotation() const
{
	return glm::eulerAngles(orientation);
}

const glm::quat& Transformable::GetQuaternion() const
{
	return orientation;
}

void Transformable::SetRotation(const glm::vec3& axis, float angle)
{
	orientation = glm::quat(glm::radians(angle), axis);
	CalculateTransformationMatrix();
}

void Transformable::SetRotation(const glm::vec3& eulerAngles)
{
	/*orientation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	orientation = glm::rotate(orientation, eulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
	orientation = glm::rotate(orientation, eulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
	orientation = glm::rotate(orientation, eulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));*/

	orientation = glm::quat(eulerAngles);
	CalculateTransformationMatrix();
}

void Transformable::Rotate(const glm::vec3& axis, float angle)
{
	orientation = glm::rotate(orientation, glm::radians(angle), axis);
	CalculateTransformationMatrix();
}

const glm::vec3& Transformable::GetScale() const
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
	transformation = glm::translate(transformation, position);
	transformation *= glm::mat4(orientation);
	transformation = glm::scale(transformation, scale);
}
