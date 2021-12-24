#pragma once

#include <lol/lol.hpp>

class Shape : public lol::Drawable, public lol::Transformable 
{
public:
	Shape();
	virtual ~Shape();
	void PreRender(const lol::CameraBase& camera) const override;
};

class Cube : public Shape
	
{
public:
	Cube();
	~Cube();
};

class Pyramid : public Shape
{
public:
	Pyramid();
	~Pyramid();
};