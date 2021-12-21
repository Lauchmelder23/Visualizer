#include "backend/Drawable.hpp"

void Drawable::Render()
{
	shader->Use();
	InitializeShader();
	vao->Render(static_cast<unsigned int>(type));
}

void Drawable::SetPrimitiveType(PrimitiveType type)
{
	this->type = type;
}
