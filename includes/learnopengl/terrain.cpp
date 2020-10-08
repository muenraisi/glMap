#include "terrain.h"
#include "debug.h"

Terrain::Terrain(float width, float height,
	Camera& camera, Shader& shader, HeightMap& heightMap) :
	width_(width), height_(height),
	camera_(&camera), shader_(&shader), heightMap_(&heightMap)
{
	glm::vec2 topLeft = glm::vec2(0, height_);
	glm::vec2 botRight = glm::vec2(width_, 0);

	glm::vec2 origin = glm::vec2(0, 0);
	boundaryTree_ = new QuadTree();
	boundaryTree_->setChildren(boundaryTree_, boundaryTree_, boundaryTree_, boundaryTree_);

	Rect rect(width / 2., height / 2., width, height);
	quadTree_ = new QuadTree(rect);
	quadTree_->topNeighbour = &boundaryTree_;
	quadTree_->botNeighbour = &boundaryTree_;
	quadTree_->leftNeighbour = &boundaryTree_;
	quadTree_->rightNeighbour = &boundaryTree_;

	transInstance_ = new std::vector<glm::vec3>;
	transInstance_->push_back(glm::vec3(width_ / 2., height_ / 2., 1.f));

	neighbourInstance_ = new std::vector<glm::vec4>;
	neighbourInstance_->push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));
}

Terrain::~Terrain()
{
	std::cout << "start destruct Terrain";
	if (quadTree_) {
		delete quadTree_;
	}
	if (boundaryTree_) {
		boundaryTree_->setChildren(nullptr, nullptr, nullptr, nullptr);
		delete boundaryTree_;
	}
	if (transInstance_) {
		delete transInstance_;
	}
	if (neighbourInstance_) {
		delete neighbourInstance_;
	}

}

void Terrain::render()
{
	//width = 2; height = 1;

	if (VAO == 0)
	{
		splitQuadTree();
		initialGLData();
	}
	else {
		updateQuadTree();
	}

	updateInstances();

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, transIBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * transInstance_->size(),
		transInstance_->data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, neighbourIBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * neighbourInstance_->size(),
		neighbourInstance_->data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElementsInstanced(GL_PATCHES, 4, GL_UNSIGNED_SHORT, (const GLvoid*)0, static_cast<GLsizei>(transInstance_->size()));
}


void Terrain::initialGLData() {
	float halfWidth = static_cast<float>(width_ / 2.);
	float halfHeight = static_cast<float>(height_ / 2.);
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
		-halfWidth, 0.f, -halfHeight,
		-halfWidth, 0.f, halfHeight,
		halfWidth, 0.f, halfHeight,
		halfWidth, 0.f, -halfHeight,
	};

	std::vector<GLushort> elems{ 0, 1, 2, 3 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &transIBO);
	glGenBuffers(1, &neighbourIBO);

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
	//clean
	quadTree_->deleteSubtrees();
	leaves_.clear();
	std::queue<QuadTree*> treeQueue;
	treeQueue.push(quadTree_);
	QuadTree* tree = nullptr;

	int now_depth = 0;
	int max_width = 1;
	int now_width = 0;
	int temp_max_width = 0;
	leaves_.push_back({});

	while (!treeQueue.empty()) {
		// from root to the deepest leaf 
		tree = treeQueue.front();
		treeQueue.pop();
		now_width += 1;
		if (now_width > max_width) {
			now_width = 1;
			now_depth += 1;
			max_width = temp_max_width;
			temp_max_width = 0;
			leaves_.push_back({});
		}
		assert(now_depth == tree->getDepth());
		if (tree == nullptr) {
			continue; //skip the error empty tree
		}
		if (tree->rect.isEmpty())
			continue; //not allowed to split
		//if (tree->isLeaf()) continue;
		if (needSplit(tree) && tree->split())
		{
			std::vector<QuadTree*> children = tree->getChildren();
			for (QuadTree* child : children) {
				treeQueue.push(child);
			}
			temp_max_width += 4;
		}
		else {
			assert(tree->isLeaf());
			leaves_[now_depth].insert(tree);
		}
	}
}

void Terrain::updateQuadTree() {
	std::vector<std::set<QuadTree*>> temp;
	temp.resize(leaves_.size() + 1);
	std::unordered_map<QuadTree*, int> visit;
	for (size_t i = 0; i != leaves_.size(); i++) {
		for (const auto& leaf : leaves_[i]) {
			if (needSplit(leaf) && leaf->split())
			{
				std::vector<QuadTree*> children = leaf->getChildren();
				for (QuadTree* child : children) {
					temp[i+1].insert(child);
				}
			}
			// if (needSplit)
			else if (leaf->parentTree == nullptr) {
				temp[i].insert(leaf);
			}
			else {
				if (visit.find(leaf->parentTree) == visit.end()) {
					visit[leaf->parentTree] = 1;
					temp[i].insert(leaf);
				}
				else {
					visit[leaf->parentTree] += 1;
					//std::cout << visit[leaf->parentTree] << std::endl;
					temp[i].insert(leaf);
					if (visit[leaf->parentTree] == 4) {
						//std::cout << "check parent tree ";
						if (!needSplit(leaf->parentTree)) {
							//std::cout << "split" << std::endl;
							for (auto child : leaf->parentTree->getChildren()) {
								temp[i].erase(child);
							}
							temp[i-1].insert(leaf->parentTree);
							visit.erase(leaf->parentTree);
							leaf->parentTree->deleteSubtrees();
						}
						//std::cout << "remain" << std::endl;
					}
				}
			}
		}
	}
	leaves_ = temp;
	if (leaves_.back().empty()) {
		leaves_.pop_back();
	}
}

void Terrain::updateInstances()
{
	transInstance_->clear();
	neighbourInstance_->clear();

	for (const auto& depth : leaves_) {
		for (const auto& leaf : depth) {
			transInstance_->push_back(leaf->getTrans());
			neighbourInstance_->push_back(leaf->getNeighbour());
		}
	}
}

bool Terrain::needSplit(const QuadTree* quadTree)
{
	//if (center.topLeft == center.botRight) return false;
	//if (center.scale <= 0.01) return false;

	Rect rect = quadTree->rect;
	// any point can be used, we prefer the top left
	glm::vec3 point0 = glm::vec3(rect.x + rect.w / 2., 0.f, rect.y + rect.h / 2.);
	glm::vec3 mid = glm::vec3(rect.x, 0.f, rect.y);

	// belong to FOV
	//glm::vec3 lookup = glm::normalize(mid - camera_->Position);
	//float cosA = glm::dot(lookup, camera_->Front);
	//float cosB = glm::dot(lookup, camera_->Front);
	//if (cosA < glm::cos(glm::radians(camera_->Zoom) * 0.6) 
	//	&& cosB < glm::cos(glm::radians(camera_->Zoom))* 0.6)
	//	return false;


	float d = distance(point0, mid);
	float l = distance(mid, camera_->Position);

	if (l / d < 2) return true;
	else return false;
}
