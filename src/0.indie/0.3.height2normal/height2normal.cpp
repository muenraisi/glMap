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

float getdata(unsigned char *data, int i, int j, int width, int height) {
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i >= height) i = height - 1;
	if (j >= width) j = width - 1;
	return (float)data[i*width + j];
}

int main(int argc, char* argv[]) {
	string inputPath, outputPath;

	if (argc != 3) {
		std::cout << "run conversion with default" << std::endl;
		std::cout << "usage: " << argv[0] << " height.png normal.png" << std::endl;
		inputPath = FileSystem::getPath("resources/china/height.png").c_str();
		outputPath = FileSystem::getPath("resources/china/normal.png").c_str();
	}
	else {
		inputPath = FileSystem::getPath(argv[1]).c_str();
		outputPath = FileSystem::getPath(argv[2]).c_str();
	}
	std::cout << "Hello, start convert height map to normal map." << std::endl;
	std::cout << "inputPath: " << inputPath << std::endl;
	std::cout << "outputPath: " << outputPath << std::endl;


	int iw, ih, n;

	int scale = 1;
	// 加载图片获取宽、高、颜色通道信息
	unsigned char *idata = stbi_load(inputPath.c_str(), &iw, &ih, &n, 0);
	std::cout << "loaded sucessfuly with (" << iw << "," << ih << ")-" << n << std::endl;

	int ow = iw / scale;
	int oh = ih / scale;
	auto *odata = (unsigned char *)malloc(ow * oh * 3);

	int index;
	/*
	7 8 9
	4 5 6
	1 2 3
	*/

	float heightScale = 4.0;
	float unit = 255./heightScale / ((float)(iw + ih) / 2.f / 10.f);

	for (int i = 0;i < ih;i++) {
		for (int j = 0;j < iw;j++) {
			index = i *iw + j;
			float h1 = getdata(idata, i - 1, j - 1, iw, ih);
			float h2 = getdata(idata, i - 1, j, iw, ih);
			float h3 = getdata(idata, i - 1, j + 1, iw, ih);
			float h4 = getdata(idata, i, j - 1, iw, ih);
			//char h5 = getdata(idata, i , j , iw, ih);
			float h6 = getdata(idata, i, j + 1, iw, ih);
			float h7 = getdata(idata, i + 1, j - 1, iw, ih);
			float h8 = getdata(idata, i + 1, j, iw, ih);
			float h9 = getdata(idata, i + 1, j + 1, iw, ih);

			float nx = (h7 - h9 + h4 - h6 + h4 - h6 + h1 - h3);
			float ny = h1 - h7 + h2 - h8 + h2 - h8 + h3 - h9;

			float mag = sqrt(nx*nx + ny *ny + 8 * 8 * unit *unit);

			odata[3 * index] = (nx / mag + 1.) / 2.*255.;
			odata[3 * index + 1] = (ny / mag + 1.) / 2.*255.;
			odata[3 * index + 2] = ((8. *unit / mag + 1.) / 2. * 255.);
		}
	}

	// 写入图片
	auto ret = stbi_write_png(outputPath.c_str(), ow, oh, 3, odata, 0);
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