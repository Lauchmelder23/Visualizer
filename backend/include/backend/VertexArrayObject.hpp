#pragma once

#include <vector>
#include <memory>

// struct representing an OpenGL attribute pointer
struct VertexAttribute
{
	int				size;
	unsigned int	type;
	bool			normalized;
	unsigned int	stride;
	const void*		pointer;
};

// Useful abbreviations
typedef std::vector<float>				VertexArray;
typedef std::vector<unsigned int>		IndexArray;
typedef std::vector<VertexAttribute>	Layout;

// OpenGL Buffer usages (I turned them into an enum so it's easier to know what options exist)
enum class Usage
{
	Static, Stream, Dynamic
};

// VAO structure that sets up the buffers and deletes them at the end of the lifecycle
class AbstractVertexArrayObject
{
	friend class VAOFactory;

public:
	AbstractVertexArrayObject() = delete;
	AbstractVertexArrayObject(const VertexArray& vertices, const IndexArray& indices, const Layout& layout, Usage usage);
	~AbstractVertexArrayObject();

	void Render(unsigned int mode = 4);

private:
	unsigned int vao, vbo, ebo;
	size_t indexCount;
};

// You cannot actually create this VAO, you are forced to use a shared pointer
// so the buffers dont get accidentally deleted while another obejct is potentially still using it.
// I find this to be very important since VAOs are supposed to be shared between copies of objects
// if they have the same model.
typedef std::shared_ptr<AbstractVertexArrayObject> VertexArrayObject;

// Factory for creating said shared pointers.
class VAOFactory
{
public:
	static VertexArrayObject Produce(const VertexArray& vertices, const IndexArray& indices, const Layout& layout, Usage usage = Usage::Static)
	{
		return std::make_shared<AbstractVertexArrayObject>(vertices, indices, layout, usage);
	}
};