#include <assert.h>

#include "quadTree.h"




QuadTree::QuadTree()
{
	topNeighbour = nullptr;
	leftNeighbour = nullptr;
	botNeighbour = nullptr;
	rightNeighbour = nullptr;

	topLeftChild_ = nullptr;
	topRightChild_ = nullptr;
	botLeftChild_ = nullptr;
	botRightChild_ = nullptr;

	parentTree = nullptr;
	depth_ = 0;
}


QuadTree::QuadTree(Rect& _rect, int _depth) :QuadTree()
{
	rect = _rect;
	depth_ = _depth;
}

QuadTree::~QuadTree()
{
	deleteSubtrees();
}


int QuadTree::getDepth() const
{
	return depth_;
}

glm::vec3 QuadTree::getTrans()
{
	return glm::vec3(rect.x, rect.y, pow(2, -depth_));
}

glm::vec4 QuadTree::getNeighbour()
{
	return glm::vec4(*topNeighbour != nullptr, *leftNeighbour != nullptr,
		*botNeighbour != nullptr, *rightNeighbour != nullptr);
}

glm::vec4 QuadTree::getHeight( HeightMap& heightMap, float scale)
{
	// TODO:
	glm::vec4 ret;
	std::vector<glm::vec2> corners = getConers();
	for (int i = 0; i < 4; i++) {
		ret[i] = heightMap.query(int(corners[i].x *scale), int(corners[i].y*scale));
	}
	return ret;
}

std::vector<glm::vec2> QuadTree::getConers()
{
	std::vector<glm::vec2> ret;
	ret.push_back(glm::vec2(rect.x + rect.w / 2., rect.y + rect.h / 2.));
	ret.push_back(glm::vec2(rect.x - rect.w / 2., rect.y + rect.h / 2.));
	ret.push_back(glm::vec2(rect.x - rect.w / 2., rect.y - rect.h / 2.));
	ret.push_back(glm::vec2(rect.x + rect.w / 2., rect.y - rect.h / 2.));
	return ret;
}

void QuadTree::deleteSubtrees()
{
	if (topLeftChild_)
	{
		delete topLeftChild_;
		topLeftChild_ = nullptr;
	}
	if (topRightChild_) {
		delete topRightChild_;
		topRightChild_ = nullptr;
	}
	if (botLeftChild_) {
		delete botLeftChild_;
		botLeftChild_ = nullptr;
	}
	if (botRightChild_) {
		delete botRightChild_;
		botRightChild_ = nullptr;
	}
}

void QuadTree::setChildren(QuadTree* topRightChild, QuadTree* topLeftChild,
	QuadTree* botLeftChild, QuadTree* botRightChild)
{
	topLeftChild_ = topLeftChild;
	topRightChild_ = topRightChild;
	botLeftChild_ = botRightChild;
	botRightChild_ = botRightChild;
}

std::vector<QuadTree*> QuadTree::getChildren()
{
	if (isLeaf())
		return std::vector<QuadTree*>();
	else
		return std::vector<QuadTree*>{ topRightChild_, topLeftChild_, botLeftChild_, botRightChild_ };
}

std::vector<QuadTree*> QuadTree::getDescendants()
{
	std::vector<QuadTree*> descendatas;

	std::queue<QuadTree*> candidates;
	candidates.push(this);
	QuadTree* now=nullptr;
	while (!candidates.empty()) {
		now = candidates.front();
		candidates.pop();
		std::vector<QuadTree*> children= now->getChildren();
		if (children.empty()) {
			descendatas.push_back(now);
		}
		else {
			for (auto tree : now->getChildren()) {
				candidates.push(tree);
			}
		}
	}
	return descendatas;
}

bool QuadTree::isLeaf()
{
	if (topLeftChild_ == nullptr || topRightChild_ == nullptr ||
		botLeftChild_ == nullptr || botRightChild_ == nullptr )
		return true;
	else
		return false;
}

bool QuadTree::isAdjacent()
{
	if (*topNeighbour == nullptr || *botNeighbour == nullptr ||
		*rightNeighbour == nullptr || *leftNeighbour == nullptr)
		return false;
	else
		return true;
}

void QuadTree::split()
{
	/*
	topLeft					topCenter
				topLeftTree_   |    topRightTree_
	centerLeft			-----center-----		centerRight
				botLeftTree_   |    botRightTree_
							botCenter			botRight
	*/

	Rect childRect(0., 0., rect.w / 2., rect.h / 2.);
	childRect.setCenter(rect.x + rect.w / 4., rect.y + rect.h / 4.);
	topRightChild_ = new QuadTree(childRect, depth_ + 1);
	topRightChild_->topNeighbour = &(*topNeighbour)->botRightChild_;
	topRightChild_->leftNeighbour = &topLeftChild_;
	topRightChild_->rightNeighbour = &(*rightNeighbour)->topLeftChild_;
	topRightChild_->botNeighbour = &botRightChild_;
	topRightChild_->parentTree = this;

	childRect.setCenter(rect.x - rect.w / 4., rect.y + rect.h / 4.);
	topLeftChild_ = new QuadTree(childRect, depth_ + 1);
	topLeftChild_->topNeighbour = &(*topNeighbour)->botLeftChild_;
	topLeftChild_->leftNeighbour = &(*leftNeighbour)->topRightChild_;
	topLeftChild_->rightNeighbour = &topRightChild_;
	topLeftChild_->botNeighbour = &botLeftChild_;
	topLeftChild_->parentTree = this;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

	childRect.setCenter(rect.x - rect.w / 4., rect.y - rect.h / 4.);
	botLeftChild_ = new QuadTree(childRect, depth_ + 1);
	botLeftChild_->topNeighbour = &topLeftChild_;
	botLeftChild_->leftNeighbour = &(*leftNeighbour)->botRightChild_;
	botLeftChild_->rightNeighbour = &botRightChild_;
	botLeftChild_->botNeighbour = &(*botNeighbour)->topLeftChild_;
	botLeftChild_->parentTree = this;

	childRect.setCenter(rect.x + rect.w / 4., rect.y - rect.h / 4.);
	botRightChild_ = new QuadTree(childRect, depth_ + 1);
	botRightChild_->topNeighbour = &topRightChild_;
	botRightChild_->leftNeighbour = &botLeftChild_;
	botRightChild_->rightNeighbour = &(*rightNeighbour)->botLeftChild_;
	botRightChild_->botNeighbour = &(*botNeighbour)->topRightChild_;
	botRightChild_->parentTree = this;
}


