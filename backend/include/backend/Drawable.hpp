#pragma once

#include <glad/glad.h>
#include "VertexArrayObject.hpp"
#include "Shader.hpp"

enum class PrimitiveType
{
	Lines = GL_LINES,
	LineStrip = GL_LINE_STRIP,
	LineLoop = GL_LINE_LOOP,

	Triangles = GL_TRIANGLES,
	TriangleStrip = GL_TRIANGLE_STRIP,
	TriangleFan = GL_TRIANGLE_FAN
};

class Drawable
{
public:
	Drawable(const Drawable& other) = delete;
	void operator=(const Drawable& other) = delete;

	virtual void InitializeShader() = 0;
	void Render();
	void SetPrimitiveType(PrimitiveType type);

protected:
	Drawable() {}

protected:
	VertexArrayObject vao;
	Shader shader;

	PrimitiveType type = PrimitiveType::Triangles;
};