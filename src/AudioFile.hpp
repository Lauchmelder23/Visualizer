#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include <SDL2/SDL_audio.h>

class AudioFile
{
public:
    AudioFile(const std::string& path);
    ~AudioFile();

    inline std::vector<float>::const_iterator begin() const { return buffer.begin(); }
    inline std::vector<float>::const_iterator end() const { return buffer.end(); }

    inline const SDL_AudioSpec& GetAudioSpec() { return spec; }
    inline uint32_t GetLength() { return length; }

    void Normalize();

private:
    SDL_AudioSpec spec;
    uint32_t length;
    std::vector<float> buffer;
};