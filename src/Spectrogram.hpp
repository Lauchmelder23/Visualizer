#pragma once

#include "Topology.hpp"
#include "AudioFile.hpp"

class Spectrogram : public Topology
{
public:
    Spectrogram(
        lol::ObjectManager& manager, 
        const glm::vec2& size, 
        const glm::uvec2& subdivision,
        const AudioFile& audio
    );

    void Update();

private:
    AudioFile audio;
    unsigned int currentStrip = 0;
};