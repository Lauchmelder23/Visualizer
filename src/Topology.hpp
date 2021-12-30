#pragma once

#include <lol/lol.hpp>
#include "Colormaps.hpp"

inline float Map(const glm::vec2& from, const glm::vec2& to, float val)
{
	return (val - from.x) * (to.y - to.x) / (from.y - from.x) + to.x;
}

class Topology : public lol::Drawable
{
public:
	Topology(lol::ObjectManager& manager, const glm::vec2& size, const glm::uvec2& subdivision);
	~Topology();

	void PreRender(const lol::CameraBase& camera) override;

	inline void SetHeightMapping(bool enable) { heightFactor = enable ? 200.0f : 0.0f; }
	inline void SetColorMapping(bool enable) { renderColor = enable; }
	inline virtual void Scroll(bool enable) { scroll = enable; }

	inline float* GetTopology() const { return (float*)image.GetPixels(); };
	inline const glm::uvec2& GetSize() const { return image.GetDimensions(); };

	void CalculateRange();
	void SetColormap(const Colormap& cm);
	void MakeTexture();

private:
	void RegisterColormap(const Colormap& cm);

protected:
	lol::Image image;
	lol::Texture2D* texture;

	lol::ObjectManager& manager;
	std::shared_ptr<lol::Texture1D> colormap;
	glm::vec2 range;

	float offset = 0.0f;
	
private:
	float heightFactor = 200.0f;
	bool renderColor = true;
	bool scroll = false;
};