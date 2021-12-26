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

	void PreRender(const lol::CameraBase& camera) override;

	inline void SetHeightMapping(bool enable) { heightFactor = enable ? 2.0f : 0.0f; }
	inline void SetColorMapping(bool enable) { colorFactor = enable ? 1.0f : 0.0f; }

	inline float* GetTopology() const { return (float*)image.GetPixels(); };
	inline const glm::uvec2& GetSize() const { return image.GetDimensions(); };
	void MakeTexture();

private:
	lol::Image image;
	lol::Texture* texture;

	float offset = 0.0f;
	float heightFactor = 2.0f;
	float colorFactor = 1.0f;
};