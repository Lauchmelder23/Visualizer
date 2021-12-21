#pragma once

#include "backend/Drawable.hpp"
#include "backend/Transformable.hpp"

class Grid : 
	public Transformable, public Drawable
{
public:
	Grid(const glm::vec2& size, unsigned int linesAlongWidth, unsigned int linesAlongHeight);

	void InitializeShader(const CameraBase& camera) const override;

private:
	glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
};