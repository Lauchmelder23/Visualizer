#include "backend/VertexArrayObject.hpp"

#include <assert.h>
#include <glad/glad.h>

AbstractVertexArrayObject::~AbstractVertexArrayObject()
{
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void AbstractVertexArrayObject::Render(GLenum mode)
{
	assert(vao != 0);

	glBindVertexArray(vao);
	// GLenum result = glGetError();
	glDrawElements(mode, indexCount, GL_UNSIGNED_INT, 0);
}

AbstractVertexArrayObject::AbstractVertexArrayObject(const VertexArray& vertices, const IndexArray& indices, const Layout& layout, Usage usage) :
	vao(0), vbo(0), ebo(0), indexCount(indices.size())
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Determing native OpenGL GLenum depending on specified usage
	GLenum bufferUsage;
	switch (usage)
	{
	case Usage::Static:		bufferUsage = GL_STATIC_DRAW; break;
	case Usage::Dynamic:	bufferUsage = GL_DYNAMIC_DRAW; break;
	case Usage::Stream:		bufferUsage = GL_STREAM_DRAW; break;

	default:	// Forgot to add a usage case to this switch
		assert("Unknown buffer usage" == "");
		break;
	}

	// Create VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), (const void*)(vertices.data()), bufferUsage);

	// Create EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (const void*)(indices.data()), bufferUsage);

	// Set up pipeline layout
	unsigned int index = 0;
	for (const VertexAttribute& attribute : layout)
	{
		glVertexAttribPointer(index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
		glEnableVertexAttribArray(index);

		index++;
	}
}
