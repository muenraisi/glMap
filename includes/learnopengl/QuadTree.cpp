#include <assert.h>

#include "QuadTree.h"




QuadTree::QuadTree()
{
	level = 1;
	topNeighbour = nullptr;
	leftNeighbour = nullptr;
	botNeighbour = nullptr;
	rightNeighbour = nullptr;

	node_ = nullptr;
	topLeftChild_ = nullptr;
	topRightChild_ = nullptr;
	botLeftChild_ = nullptr;
	botRightChild_ = nullptr;
}



QuadTree::QuadTree(Point topL, Point botR, double scale = 1.0)
{
	level = 1;
	topNeighbour = nullptr;
	leftNeighbour = nullptr;
	botNeighbour = nullptr;
	rightNeighbour = nullptr;

	node_ = new QuadNode(topL, botR, scale);
	topLeftChild_ = nullptr;
	topRightChild_ = nullptr;
	botLeftChild_ = nullptr;
	botRightChild_ = nullptr;
}

QuadTree::~QuadTree()
{
	if (node_) {
		delete node_;
	}
	if (topLeftChild_) {
		delete topLeftChild_;
	}
	if (topRightChild_) {
		delete topRightChild_;
	}
	if (botLeftChild_) {
		delete botLeftChild_;
	}
	if (botRightChild_) {
		delete botRightChild_;
	}
}

glm::vec3 QuadTree::getTrans()
{
	return glm::vec3(node_->center.GetX(), node_->center.GetY(), node_->scale);
}

glm::vec4 QuadTree::getNeigh()
{
	return glm::vec4(*topNeighbour!=nullptr, *leftNeighbour!=nullptr, 
		*botNeighbour!=nullptr, *rightNeighbour!=nullptr);
}

void QuadTree::deleteSubtrees()
{
	if (topLeftChild_ )
	{
		delete topLeftChild_;
		topLeftChild_ = nullptr;
	}
	if (topRightChild_ ) {
		delete topRightChild_;
		topRightChild_ = nullptr;
	}
	if (botLeftChild_ ) {
		delete botLeftChild_;
		botLeftChild_ = nullptr;
	}
	if (botRightChild_) {
		delete botRightChild_;
		botRightChild_ = nullptr;
	}
}

void QuadTree::setChildren(QuadTree* topLeftChild, QuadTree* topRightChild,
	QuadTree* botLeftChild, QuadTree* botRightChild)
{
	topLeftChild_ = topLeftChild;
	topRightChild_ = topRightChild;
	botLeftChild_ = botRightChild;
	botRightChild_ = botRightChild;
}

std::vector<QuadTree*> QuadTree::getChildren()
{
	return std::vector<QuadTree*>{ topLeftChild_, topRightChild_, botLeftChild_, botRightChild_ };
}

bool QuadTree::empty()
{
	if (!topLeftChild_ || !topRightChild_ || !botLeftChild_ || !botRightChild_)
		return true;
	else
		return false;
}

bool QuadTree::split()
{
	/*
	topLeft					topCenter
				topLeftTree_   |    topRightTree_
	centerLeft			-----center-----		centerRight
				botLeftTree_   |    botRightTree_
							botCenter			botRight
	*/
	if (!*topNeighbour || !*botNeighbour ||
		!*rightNeighbour || !*leftNeighbour)
		return false;

	topLeftChild_ = new QuadTree(node_->topLeft, node_->center, node_->scale / 2.);
	topLeftChild_->level = level + 1;
	topLeftChild_->topNeighbour = &(*topNeighbour)->botLeftChild_;
	topLeftChild_->leftNeighbour = &(*leftNeighbour)->topRightChild_;
	topLeftChild_->rightNeighbour = &topRightChild_;
	topLeftChild_->botNeighbour = &botLeftChild_;

	Point topCenter = Point(node_->center.GetX(), node_->topLeft.GetY());
	Point centerRight = Point(node_->botRight.GetX(), node_->center.GetY());
	topRightChild_ = new QuadTree(topCenter, centerRight, node_->scale / 2.);
	topRightChild_->level = level + 1;
	topRightChild_->topNeighbour = &(*topNeighbour)->botRightChild_;
	topRightChild_->leftNeighbour = &topLeftChild_;
	topRightChild_->rightNeighbour = &(*rightNeighbour)->topLeftChild_;
	topRightChild_->botNeighbour = &botRightChild_;

	Point centerLeft = Point(node_->topLeft.GetX(), node_->center.GetY());
	Point botCenter = Point(node_->center.GetX(), node_->botRight.GetY());
	botLeftChild_ = new QuadTree(centerLeft, botCenter, node_->scale / 2.);
	botLeftChild_->level = level + 1;
	botLeftChild_->topNeighbour = &topLeftChild_;
	botLeftChild_->leftNeighbour = &(*leftNeighbour)->botRightChild_;
	botLeftChild_->rightNeighbour = &botRightChild_;
	botLeftChild_->botNeighbour = &(*botNeighbour)->topLeftChild_;

	botRightChild_ = new QuadTree(node_->center, node_->botRight, node_->scale / 2.);
	botRightChild_->level = level + 1;
	botRightChild_->topNeighbour = &topRightChild_;
	botRightChild_->leftNeighbour = &botLeftChild_;
	botRightChild_->rightNeighbour = &(*rightNeighbour)->botLeftChild_;
	botRightChild_->botNeighbour = &(*botNeighbour)->topRightChild_;

	return true;
}


