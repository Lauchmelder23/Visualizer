#pragma once

#include <lol/lol.hpp>

inline float Map(const glm::vec2& from, const glm::vec2& to, float val)
{
	return (val - from.x) * (to.y - to.x) / (from.y - from.x) + to.x;
}

class Topology : public lol::Drawable
{
public:
	Topology(const glm::vec2& size, const glm::uvec2& subdivision);
	~Topology();

	void PreRender(const lol::CameraBase& camera) const override;

	inline float* GetTopology() const { return (float*)image.GetPixels(); };
	inline const glm::uvec2& GetSize() const { return image.GetDimensions(); };
	void MakeTexture();

private:
	lol::Image image;
	lol::Texture* texture;
};