#pragma once

#include <iostream>
#include <assert.h>
#include <stb_image.h>

class HeightMap
{
public:
	HeightMap(char const* path);
	~HeightMap();

	float query(int x, int y);
private:
	int width_;
	int height_;
	unsigned char* data_;
};