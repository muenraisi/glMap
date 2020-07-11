#include "terrain.h"


Terrain::Terrain(float width, float height) : width_(width), height_(height)
{
	Point topLeft = Point(0, height_);
	Point botRight = Point(width_, 0);
	quadTree_ = new QuadTree(topLeft, botRight, 1.0);

	instanceTrans_ = new std::vector<glm::vec3>;
	glm::vec3 transform(width_ /2., height_ /2., 1.f);
	instanceTrans_->push_back(transform);
}

void Terrain::render(Shader& shader, Camera& camera)
{
	//width = 2; height = 1;

	splitQuadTree(camera);

	if (VAO == 0)
	{
		initial(shader);
	}

	updateInstances(shader);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * instanceTrans_->size(), &(*instanceTrans_)[0], GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * instanceTrans_->size(), instanceTrans_->data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElementsInstanced(GL_PATCHES, 4, GL_UNSIGNED_SHORT, (const GLvoid*)0, instanceTrans_->size());
}


void Terrain::initial(Shader& shader) {
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

	shader.use();
	shader.setVec3("aTangent", tangent);
	shader.setVec3("aNormal", nm);
	shader.setVec3("aBitangent", bitangent);

	std::vector<float> worldVertices{
		-width_ / 2.f,0.f,-height_ / 2.f,
		width_ / 2.f,0.f,-height_ / 2.f,
		-width_ / 2.f,0.f,height_ / 2.f,
		width_ / 2.f,0.f,height_ / 2.f
	};

	std::vector<GLushort> elems{ 0, 2, 3, 1 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * worldVertices.size(), worldVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


}

void Terrain::splitQuadTree(Camera& camera)
{
	splitQuadTree(*quadTree_, camera);
}

void Terrain::splitQuadTree(QuadTree& quadTree, Camera& camera)
{
	if (quadTree.getNode() == nullptr) return;
	if (needSplit(*(quadTree.getNode()), camera))
		quadTree.split();
	else
		return;
	std::vector<QuadTree*>* trees = quadTree.getTrees();
	for (QuadTree* tree : *trees) {
		splitQuadTree(*tree, camera);
	}
}

void Terrain::updateInstances(Shader& shader)
{
	instanceTrans_->clear();
	std::stack<QuadTree*> treeStack;
	treeStack.push(quadTree_);
	QuadTree* tree = nullptr;
	while (!treeStack.empty()) {
		tree = treeStack.top();
		treeStack.pop();
		if (tree->empty()) {
			instanceTrans_->push_back(tree->getTrans());
		}
		else {
			for (QuadTree* subTree : *(tree->getTrees())) {
				treeStack.push(subTree);
			}
		}
	}
}

bool Terrain::needSplit(QuadNode& center, Camera& camera)
{
	

	//if (center.scale <= 0.01) return false;
	glm::vec3 pointA = glm::vec3(center.topLeft.GetX(), 0.f, center.topLeft.GetY());
	glm::vec3 pointB = glm::vec3(center.botRight.GetX(), 0.f, center.botRight.GetY());
	glm::vec3 mid = glm::vec3(center.center.GetX(), 0.f, center.center.GetY());
	float d = distance(pointA, pointB);
	float l = distance(mid, camera.Position);

	//glm::vec4 clipA = mvp * pointA;
	//glm::vec4 clipB = mvp * pointB;

	//glm::vec3 ndcA = glm::vec3(clipA.x, clipA.y, clipA.z) / clipA.w;
	//glm::vec3 ndcB = glm::vec3(clipB.x, clipB.y, clipB.z) / clipB.w;

	//if ((abs(ndcA.x) > 1 || abs(ndcA.z) > 1) && (abs(ndcB.x) > 1 || abs(ndcB.z) > 1)) // both of the two point out of the NDC range
	//	return false;

	//float absDistance = sqrt((ndcA.x - ndcB.x) * (ndcA.x - ndcB.x) + (ndcA.z - ndcB.z) * (ndcA.z - ndcB.z));

	if (l/d < 4) return true;
	else return false;
}
