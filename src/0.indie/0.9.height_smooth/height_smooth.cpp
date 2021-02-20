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

float* Uchar2Float(unsigned char* data, int w, int h) {
  auto* ndata = (float*)malloc(w * h * sizeof(float));
  for (int x = 0; x < h;x++) {
    for (int y = 0; y < w;y++) {
      ndata[w * x + y] = static_cast<float> (data[w * x + y]);
    }
  }
  return ndata;
}

unsigned char* Float2Uchar(float* data, int w, int h) {
  auto* ndata = (unsigned char*)malloc(w * h * sizeof(unsigned char));
  for (int x = 0; x < h;x++) {
    for (int y = 0; y < w;y++) {
      ndata[w * x + y] = static_cast<unsigned char> (data[w * x + y]);
    }
  }
  return ndata;
}

void SmoothTerrain(float* data, int passes, int w, int h)
{
	auto* ndata = (float*)malloc(w * h *sizeof(float));

	int count = 0;
  while (passes > 0)
  {
		count++;
		std::cout << "Start " << count << "th iteration of smooth." << std::endl;
    passes--;

    // Note: MapWidth and MapHeight should be equal and power-of-two values 

    for (int x = 0; x < h; x++)
    {
      for (int y = 0; y < w; y++)
      {
        int adjacentSections = 0;
        float sectionsTotal = 0.0f;

        if ((x - 1) > 0) // Check to left
        {
          sectionsTotal += data[(x - 1)*w+ y];
          adjacentSections++;

          if ((y - 1) > 0) // Check up and to the left
          {
            sectionsTotal += data[(x - 1)*w+ y - 1];
            adjacentSections++;
          }

          if ((y + 1) < w) // Check down and to the left
          {
            sectionsTotal += data[(x - 1)*w+ y + 1];
            adjacentSections++;
          }
        }

        if ((x + 1) < h) // Check to right
        {
          sectionsTotal += data[(x + 1)*w+ y];
          adjacentSections++;

          if ((y - 1) > 0) // Check up and to the right
          {
            sectionsTotal += data[(x + 1)*w+y - 1];
            adjacentSections++;
          }

          if ((y + 1) < w) // Check down and to the right
          {
            sectionsTotal += data[(x + 1)*w+ y + 1];
            adjacentSections++;
          }
        }

        if ((y - 1) > 0) // Check above
        {
          sectionsTotal += data[x*w+ y - 1];
          adjacentSections++;
        }

        if ((y + 1) < w) // Check below
        {
          sectionsTotal += data[x*w+y + 1];
          adjacentSections++;
        }

				ndata[x * w + y] = (data[x*w+y] + (sectionsTotal / adjacentSections)) * 0.5f;
      }
    }

    // Overwrite the HeightData info with our new smoothed info
    for (int x = 0; x < h; x++)
    {
      for (int y = 0; y < w; y++)
      {
        data[x*w+y] = ndata[x * w + y];
      }
    }
  }
  free(ndata);
}


int main(int argc, char* argv[]) {
	string inputPath, outputPath;

	if (argc != 3) {
		std::cout << "run conversion with default" << std::endl;
		std::cout << "usage: " << argv[0] << " height.png normal.png" << std::endl;
		//inputPath = FileSystem::getPath("resources/china/height.png").c_str();
		//outputPath = FileSystem::getPath("resources/china/height_smooth.png").c_str();
    inputPath = "D:/indie/data/SRTM_NE_250m_TIF/china-std.png";
    
	}
	else {
		inputPath = FileSystem::getPath(argv[1]).c_str();
		outputPath = FileSystem::getPath(argv[2]).c_str();
	}
	std::cout << "Hello, start smooth the height." << std::endl;
	std::cout << "inputPath: " << inputPath << std::endl;
	std::cout << "outputPath: " << outputPath << std::endl;




	int iw, ih, n;

	int scale = 1; //max 240
	// 加载图片获取宽、高、颜色通道信息
	unsigned char *idata = stbi_load(inputPath.c_str(), &iw, &ih, &n, 0);
	std::cout << "loaded sucessfuly with (" << iw << "," << ih << ")-" << n << std::endl;


	/*
	7 8 9
	4 5 6
	1 2 3
	*/

	float* ndata = Uchar2Float(idata, iw, ih);
  stbi_image_free(idata);

  int count = 0 ;
  while (true) {
    count++;
    SmoothTerrain(ndata, 100, iw, ih);
    unsigned char* odata = Float2Uchar(ndata, iw, ih);
    int ow = iw;
    int oh = ih;


    // 写入图片
    outputPath = "D:/indie/data/SRTM_NE_250m_TIF/china-std-s"+ std::to_string(count) +"00.png";
    auto ret = stbi_write_png(outputPath.c_str(), ow, oh, 1, odata, 0);
    if (ret == 0) {
      std::cout << "Unsuccessful ";
    }
    else {
      std::cout << "Successful ";
    }
    std::cout << "saved smooth png!" << std::endl;
    stbi_image_free(odata);
  }

  stbi_image_free(ndata);
	
	return 0;
}