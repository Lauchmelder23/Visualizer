#pragma once

#include <lol/lol.hpp>

class Shape : public lol::Drawable, public lol::Transformable 
{
public:
	Shape(const std::shared_ptr<lol::Texture>& texture);
	virtual ~Shape();
	void PreRender(const lol::CameraBase& camera) const override;

protected:
	std::shared_ptr<lol::Texture> texture;
};

class Cube : public Shape
	
{
public:
	Cube(const std::shared_ptr<lol::Texture>& texture);
	~Cube();
};

class Pyramid : public Shape
{
public:
	Pyramid(const std::shared_ptr<lol::Texture>& texture);
	~Pyramid();
};