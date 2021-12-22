#pragma once

#include <glad/glad.h>
#include "VertexArrayObject.hpp"
#include "Shader.hpp"

class CameraBase;

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

	virtual void PreRender(const CameraBase& camera) const { };
	void Draw(const CameraBase& camera) const;
	void SetPrimitiveType(PrimitiveType type);

protected:
	Drawable() {}

protected:
	VertexArrayObject vao;
	Shader shader;

	PrimitiveType type = PrimitiveType::Triangles;
};