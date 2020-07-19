#pragma once

#include <vector>
#include <queue>
#include <glm/glm.hpp>

#include "camera.h"
#include "shader_i.h"
#include "QuadTree.h"


// here we define a class on render the terrain
class Terrain {
public:
	Terrain(float width, float height,  Camera& camera , Shader& shader) ;
	~Terrain();
	void render();
	void initial();
	void splitQuadTree();
	void updateInstances();

	bool needSplit(const QuadNode& center);


private:
	float width_;
	float height_;
	Camera* camera_;
	Shader* shader_;
	int max_level_ = 5;
	QuadTree* quadTree_;
	QuadTree* boundaryTree_;
	std::vector<glm::vec3>* transInstance_;
	std::vector<glm::vec4>* neighInstance_;


	unsigned int VAO=0;
	unsigned int VBO, EBO, transIBO, neighIBO;

};


