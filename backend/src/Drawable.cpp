#include "backend/Drawable.hpp"

void Drawable::Draw(const CameraBase& camera) const
{
	shader->Use();
	PreRender(camera);
	vao->Render(static_cast<unsigned int>(type));
}

void Drawable::SetPrimitiveType(PrimitiveType type)
{
	this->type = type;
}
