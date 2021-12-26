#pragma once

#include <map>
#include <vector>
#include <string>

struct Colormap
{
    unsigned int id;
    std::vector<float> data;
};

extern std::vector<const char*> colormapNames;
extern std::vector<Colormap> colormaps;