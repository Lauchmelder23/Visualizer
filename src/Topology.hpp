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
	inline void SetColorMapping(bool enable) { renderColor = enable; }
	inline void Scroll(bool enable) { scroll = enable; }

	inline float* GetTopology() const { return (float*)image.GetPixels(); };
	inline const glm::uvec2& GetSize() const { return image.GetDimensions(); };
	void MakeTexture();

private:
	lol::Image image;
	lol::Texture2D* texture;
	std::shared_ptr<lol::Texture1D> colormap;

	float offset = 0.0f;
	float heightFactor = 2.0f;
	bool renderColor = true;
	bool scroll = false;
	glm::vec2 range;
};