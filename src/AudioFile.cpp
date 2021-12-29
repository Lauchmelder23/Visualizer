#include "AudioFile.hpp"

#include <iostream>

AudioFile::AudioFile(const std::string& path)
{
    // Load the file from disk
    uint8_t* tempBuf = nullptr;
    if(SDL_LoadWAV(path.c_str(), &spec, &tempBuf, &length) == NULL)
    {
        std::cerr << "Failed to load audio file \"" << path << "\": " << SDL_GetError() << std::endl;
        return;
    }

    // Copy raw audio buffer over into vector
    // Perform conversion to float if necessary
    unsigned int sampleSize = SDL_AUDIO_BITSIZE(spec.format);
    bool isFloat = SDL_AUDIO_ISFLOAT(spec.format);
    for(uint8_t* ptr = tempBuf; ptr < tempBuf + length; ptr += sampleSize)
    {
        if(isFloat)
            buffer.push_back(*((float*)ptr));
        else
            buffer.push_back((float)*ptr);
    }

    // Free the audio buffer
    SDL_FreeWAV(tempBuf);
}

AudioFile::~AudioFile()
{
    
}

void AudioFile::Normalize()
{
    float largestVal = 0.0f;
    for(const float& sample : buffer)
        largestVal = std::max(largestVal, std::abs(sample));
    
    float normFactor = 1.0f / largestVal;
    for(float& sample : buffer)
        sample *= normFactor;
}