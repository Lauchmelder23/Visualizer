#pragma once

#include <glm/glm.hpp>
#include "backend/VertexArrayObject.hpp"
#include "backend/Shader.hpp"

/**
 * A cuboid that sits at a position and expands into all
 * three spatial directions
 */
class Cuboid
{
public:
	Cuboid();

	void Render();

private:
	VertexArrayObject vao;
	Shader shader;

	// TODO: Remove view and projection matrices from cube class
	glm::mat4 model, view, perspective;
};