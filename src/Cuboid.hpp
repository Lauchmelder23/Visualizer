#pragma once

#include <glm/glm.hpp>
#include "backend/Drawable.hpp"
#include "backend/Transformable.hpp"

/**
 * A cuboid that sits at a position and expands into all
 * three spatial directions
 */
class Cuboid :
	public Drawable, public Transformable
{
public:
	Cuboid();

	void InitializeShader() override;

private:

	// TODO: Remove view and projection matrices from cube class
	glm::mat4 view, perspective;
};