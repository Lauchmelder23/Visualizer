#include "ScrollingPlot.hpp"

ScrollingPlot::ScrollingPlot(
    lol::ObjectManager& manager, 
    const glm::vec2& size, 
    const glm::uvec2& subdivision, 
    const glm::vec2& domain,
    float temporalResolution,
    std::function<float(float, float)> func
    ):
    Topology(manager, size, subdivision), domain(domain), dt(temporalResolution), func(func)
{
    // Create initial texture for t = 0
    for (unsigned int x = 0; x < subdivision.x; x++)
    {
        CalculateStrip(x);
        t += dt;
    }

    range = glm::vec2(-1.0f, 1.0f);
    MakeTexture();
}

void ScrollingPlot::StepForward(unsigned int steps)
{
    for(unsigned int n = 0; n < steps; n++)
    {
        t += dt;
        CalculateStrip(currentStrip);
        currentStrip = (currentStrip + 1) % image.GetDimensions().x;

        offset += 1.0f / (float)image.GetDimensions().x;
    }
    MakeTexture();
}

void ScrollingPlot::CalculateStrip(unsigned int strip)
{
    float* pixels = GetTopology();
    glm::uvec2 size = image.GetDimensions();

    for(unsigned int y = 0; y < size.y; y++)
    {
        pixels[y * size.x + strip] = func(
            t,
            Map(glm::vec2(0.0f, size.y), domain, y)
        );
    }
}