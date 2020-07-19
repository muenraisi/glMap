#include "terrain.h"
#include "debug.h"

Terrain::Terrain(float width, float height,  Camera& camera, Shader& shader):
	width_(width), height_(height), camera_(&camera), shader_(&shader)
{
	glm::vec2 topLeft = glm::vec2(0, height_);
	glm::vec2 botRight = glm::vec2(width_, 0);
	
	glm::vec2 origin = glm::vec2(0, 0);
	boundaryTree_ = new QuadTree(origin, origin, 0.0);
	boundaryTree_->setChildren(boundaryTree_, boundaryTree_, boundaryTree_, boundaryTree_);

	quadTree_ = new QuadTree(topLeft, botRight, 1.0);
	quadTree_->topNeighbour = &boundaryTree_;
	quadTree_->botNeighbour = &boundaryTree_;
	quadTree_->leftNeighbour = &boundaryTree_;
	quadTree_->rightNeighbour = &boundaryTree_;


	transInstance_ = new std::vector<glm::vec3>;
	transInstance_->push_back(glm::vec3(width_ / 2., height_ / 2., 1.f));

	neighInstance_ = new std::vector<glm::vec4>;
	neighInstance_->push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));
}

Terrain::~Terrain()
{
	if (quadTree_) {
		delete quadTree_;
	}
	if (boundaryTree_ ) {
		boundaryTree_->setChildren(nullptr, nullptr, nullptr, nullptr);
		delete boundaryTree_;
	}
	if (transInstance_ ) {
		delete transInstance_;
	}
	if (neighInstance_) {
		delete neighInstance_;
	}
}

void Terrain::render()
{
	//width = 2; height = 1;

	splitQuadTree();

	if (VAO == 0)
	{
		initial();
	}

	updateInstances();

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, transIBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * transInstance_->size(), 
		transInstance_->data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, neighIBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * neighInstance_->size(), 
		neighInstance_->data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElementsInstanced(GL_PATCHES, 4, GL_UNSIGNED_SHORT, (const GLvoid*)0, transInstance_->size());
}


void Terrain::initial() {
	double halfWidth = width_ / 2.;
	double halfHeight = height_ / 2.;
	// positions
	glm::vec3 pos1(halfWidth, 0.0f, halfHeight);
	glm::vec3 pos2(-halfWidth, 0.0f, halfHeight);
	glm::vec3 pos3(-halfWidth, 0.0f, -halfHeight);
	glm::vec3 pos4(halfWidth, 0.0f, -halfHeight);
	// texture coordinates
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);
	// normal vector
	glm::vec3 nm(0.0f, 1.0f, 0.0f);

	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent, bitangent;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);

	shader_->use();
	shader_->setVec3("aTangent", tangent);
	shader_->setVec3("aNormal", nm);
	shader_->setVec3("aBitangent", bitangent);

	std::vector<float> worldVertices{
		-width_ / 2.f, 0.f, -height_ / 2.f,
		width_ / 2.f, 0.f, -height_ / 2.f,
		-width_ / 2.f, 0.f, height_ / 2.f,
		width_ / 2.f, 0.f, height_ / 2.f
	};

	std::vector<GLushort> elems{ 0, 2, 3, 1 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &transIBO);
	glGenBuffers(1, &neighIBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * worldVertices.size(), worldVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Terrain::splitQuadTree()
{
	quadTree_->deleteSubtrees();

	std::queue<QuadTree*> treeQueue;
	treeQueue.push(quadTree_);
	QuadTree* tree = nullptr;
	while (!treeQueue.empty()) {
		tree = treeQueue.front();
		treeQueue.pop();
		if (!tree) continue;
		if (!tree->getNode()) continue;
		if (needSplit(*(tree->getNode())) && !tree->split())
			continue;
		std::vector<QuadTree*> children = tree->getChildren();
		for (QuadTree* child : children) {
			treeQueue.push(child);
		}
	}
}

void Terrain::updateInstances()
{
	transInstance_->clear();
	neighInstance_->clear();
	std::queue<QuadTree*> treeQueue;
	treeQueue.push(quadTree_);
	QuadTree* tree = nullptr;
	while (!treeQueue.empty()) {
		tree = treeQueue.front();
		treeQueue.pop();
		if (!tree)
			continue;
		if (tree->empty()) {
			transInstance_->push_back(tree->getTrans());
			neighInstance_->push_back(tree->getNeigh());
		}
		else {
			for (QuadTree* subTree : tree->getChildren()) {
				treeQueue.push(subTree);
			}
		}
	}
}

bool Terrain::needSplit(const  QuadNode& center)
{

	//if (center.scale <= 0.01) return false;
	glm::vec3 pointA = glm::vec3(center.topLeft.x, 0.f, center.topLeft.y);
	glm::vec3 pointB = glm::vec3(center.botRight.x, 0.f, center.botRight.y);
	glm::vec3 mid = glm::vec3(center.center.x, 0.f, center.center.y);
	float d = distance(pointA, pointB);
	float l = distance(mid, camera_->Position);

	if (l/d < 16) return true;
	else return false;
}
