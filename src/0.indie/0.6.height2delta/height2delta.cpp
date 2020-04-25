#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <learnopengl/filesystem.h>

using namespace std;

int maxOf(int a, int b) {
	return a > b ? a : b;
}

int minOf(int a, int b) {
	return a < b ? a : b;
}

float getDelta(unsigned char *data, int x, int y, int width, int height,int range) {
	float min = 255,max = 0;
	range = 100;
	for (int i = maxOf(x - range, 0);i < minOf(x + range, height);i++) {
		for (int j = maxOf(y - range, 0);j < minOf(y + range, width);j++) {
			float value = data[i*width+j];
			if (value < min) min = value;
			if (value > max) max = value;
		}
	}
	return max-min;
}

int main(int argc, char* argv[]) {
	string inputPath, outputPath;

	if (argc != 3) {
		std::cout << "run conversion with default" << std::endl;
		std::cout << "usage: " << argv[0] << " height.png normal.png" << std::endl;
		inputPath = FileSystem::getPath("resources/china/height.png").c_str();
		outputPath = FileSystem::getPath("resources/china/delta.png").c_str();
	}
	else {
		inputPath = FileSystem::getPath(argv[1]).c_str();
		outputPath = FileSystem::getPath(argv[2]).c_str();
	}
	std::cout << "Hello, start convert height map to normal map." << std::endl;
	std::cout << "inputPath: " << inputPath << std::endl;
	std::cout << "outputPath: " << outputPath << std::endl;


	int iw, ih, n;

	int scale = 10; //max 240
	// 加载图片获取宽、高、颜色通道信息
	unsigned char *idata = stbi_load(inputPath.c_str(), &iw, &ih, &n, 0);
	std::cout << "loaded sucessfuly with (" << iw << "," << ih << ")-" << n << std::endl;

	int ow = iw / scale;
	int oh = ih / scale;
	auto *odata = (unsigned char *)malloc(ow * oh );

	int iPos,oPos;
	/*
	7 8 9
	4 5 6
	1 2 3
	*/


	for (int i = 0;i < oh;i++) {
		
		for (int j = 0;j < ow;j++) {
			
			oPos = i*ow + j;
			float delta = getDelta(idata, i*scale, j*scale, iw, ih, scale);
			odata[oPos] = delta;
			std::cout << "+";
		}
		std::cout << std::endl << i << " out of " << oh << std::endl;
	}

	// 写入图片
	auto ret = stbi_write_png(outputPath.c_str(), ow, oh, 1, odata, 0);
	if (ret == 0) {
		std::cout << "Unsuccessful ";
	}
	else {
		std::cout << "Successful ";
	}
	std::cout << "saved normal png!" << std::endl;

	stbi_image_free(idata);
	stbi_image_free(odata);
	return 0;
}