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
	Terrain(float width, float height) ;
	~Terrain();
	void render(Shader& shader, Camera& camera);
	void initial(Shader& shader);
	void splitQuadTree(Camera& camera);
	void updateInstances(Shader& shader);

	bool needSplit(QuadNode& center, Camera& camera);


private:
	float width_;
	float height_;
	int max_level_ = 5;
	QuadTree* quadTree_;
	QuadTree* boundaryTree_;
	std::vector<glm::vec3>* transInstance_;
	std::vector<glm::vec4>* neighInstance_;


	unsigned int VAO=0;
	unsigned int VBO, EBO, transIBO, neighIBO;

};

