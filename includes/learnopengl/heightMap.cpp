#include "heightMap.h"

HeightMap::HeightMap(char const* path)
{
	int nrComponents;
	data_ = stbi_load(path, &width_, &height_, &nrComponents, 0);
	if (data_)
	{
		assert(nrComponents == 1);
		std::cout << path << " loaded sucessfuly with ("
			<< width_ << "," << height_ << ")-" << nrComponents << std::endl;
	}
	else {
		std::cout << path << " loaded failed " << std::endl;
	}
}

HeightMap::~HeightMap()
{
	stbi_image_free(data_);
}

// qure the height based on the pixel coordinate
float HeightMap::query(int x, int y)
{
	x = x % height_;
	y = y % width_;
	return (float)data_[x * width_ + y];
}
