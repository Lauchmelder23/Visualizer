#pragma once

#include <functional>

#include <lol/lol.hpp>

typedef std::function<float(float, float)> PlottableFunction;

class Plot3D :
	public lol::Transformable, public lol::Drawable
{
public:
	Plot3D(const lol::BBox& domainAndRange, float scale, float resolution, PlottableFunction func);

private:
	void PreRender(const lol::CameraBase& camera) const override;
};