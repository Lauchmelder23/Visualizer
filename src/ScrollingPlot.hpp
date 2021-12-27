#pragma once

#include <functional>
#include <lol/lol.hpp>

#include "Topology.hpp"

class ScrollingPlot : public Topology
{
public:
    ScrollingPlot(
        lol::ObjectManager& manager, 
        const glm::vec2& size, const glm::uvec2& subdivision, 
        const glm::vec2& domain,
        float temporalResolution,
        std::function<float(float, float)> func
    );

    void StepForward(unsigned int steps);
    inline virtual void Scroll(bool enable) override {}

private:
    void CalculateStrip(unsigned int strip);

private:
    float t = 0.0f;
    float dt = 0.0f;
    unsigned currentStrip = 0;

    glm::vec2 domain;
    std::function<float(float, float)> func;
};