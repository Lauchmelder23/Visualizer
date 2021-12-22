#pragma once

#include <functional>

#include "backend/Drawable.hpp"
#include "backend/Transformable.hpp"
#include "backend/BoundingBox.hpp"

typedef std::function<float(float, float)> PlottableFunction;

class Plot3D :
	public Transformable, public Drawable
{
public:
	Plot3D(const BBox& domainAndRange, float scale, float resolution, PlottableFunction func);

private:
	void PreRender(const CameraBase& camera) const override;
};