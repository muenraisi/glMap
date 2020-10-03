#include "heightMap.h"

HeightMap::HeightMap(char const* path)
{
	int nrComponents;
	data_ = stbi_load(path, &width_, &height_, &nrComponents, 0);
	assert(width_ > 0 && height_ > 0);
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
float HeightMap::query(int x, int y) const
{
	//std::cout << width_ << height_ << std::endl;
	x = x % width_;
	y = y % height_;
	//std::cout << "quer value of ( " << x << ", " << y << " ) is: " 
	//	<< (float)data_[x * width_ + y] / 255. << std::endl;
	float value = static_cast<float>(data_[y * width_ + x] / 255.);
	return value;
}
