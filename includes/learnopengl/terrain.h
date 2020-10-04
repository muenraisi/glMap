#pragma once

#include <vector>
#include <queue>
#include <glm/glm.hpp>

#include "camera.h"
#include "shader_i.h"
#include "heightMap.h"
#include "QuadTree.h"
#include "debug.h"


// here we define a class on render the terrain
class Terrain {
public:
	Terrain(float width, float height, Camera& camera, Shader& shader, HeightMap& heightMap);
	~Terrain();
	void render();
	void initial();
	void splitQuadTree(); // obtain the childs along with 
	void updateInstances();

	bool needSplit(const Rect& center);


private:
	float width_;
	float height_;
	Camera* camera_;
	Shader* shader_;
	HeightMap* heightMap_;
	int max_level_ = 10;
	QuadTree* quadTree_;
	QuadTree* boundaryTree_;
	std::vector<glm::vec3>* transInstance_;
	std::vector<glm::vec4>* neighbourInstance_;

	std::vector<QuadTree*> leaves_;

	unsigned int VAO=0;
	unsigned int VBO, EBO, transIBO, neighbourIBO;

};


