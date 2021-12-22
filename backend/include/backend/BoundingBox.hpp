#pragma once

struct Rect
{
	float x, y;
	float w, h;
};

struct BoundingBox
{
	float x, y, z;
	float w, h, d;
};

typedef BoundingBox BBox;