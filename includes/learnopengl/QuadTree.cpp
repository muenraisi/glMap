#include <assert.h>

#include "quadTree.h"




QuadTree::QuadTree()
{
	level = 1;
	topNeighbour = nullptr;
	leftNeighbour = nullptr;
	botNeighbour = nullptr;
	rightNeighbour = nullptr;

	node_ = QuadNode();
	topLeftChild_ = nullptr;
	topRightChild_ = nullptr;
	botLeftChild_ = nullptr;
	botRightChild_ = nullptr;
}



QuadTree::QuadTree(QuadNode& quadNode)
{
	level = 1;
	topNeighbour = nullptr;
	leftNeighbour = nullptr;
	botNeighbour = nullptr;
	rightNeighbour = nullptr;

	node_ = quadNode;
	topLeftChild_ = nullptr;
	topRightChild_ = nullptr;
	botLeftChild_ = nullptr;
	botRightChild_ = nullptr;
}

QuadTree::~QuadTree()
{
	deleteSubtrees();
}

QuadNode* QuadTree::getNode()
{
	return &node_;
}

glm::vec3 QuadTree::getTrans()
{
	return glm::vec3(node_.center.x, node_.center.y, node_.scale);
}

glm::vec4 QuadTree::getNeighbour()
{
	return glm::vec4(*topNeighbour!=nullptr, *leftNeighbour!=nullptr, 
		*botNeighbour!=nullptr, *rightNeighbour!=nullptr);
}

glm::vec4 QuadTree::getHeight()
{
	// TODO:
	return glm::vec4(0.f,0.f,0.f,0.f);
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

void QuadTree::setChildren( QuadTree* topRightChild,  QuadTree* topLeftChild,
	 QuadTree* botLeftChild,  QuadTree* botRightChild)
{
	topLeftChild_ = topLeftChild;
	topRightChild_ = topRightChild;
	botLeftChild_ = botRightChild;
	botRightChild_ = botRightChild;
}

std::vector<QuadTree*> QuadTree::getChildren()
{
	return std::vector<QuadTree*>{ topRightChild_, topLeftChild_, botLeftChild_, botRightChild_ };
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
	QuadNode tempNode;
	glm::vec2 topCenter = glm::vec2(node_.center.x, node_.topLeft.y);
	glm::vec2 centerRight = glm::vec2(node_.botRight.x, node_.center.y);
	tempNode = QuadNode(topCenter, centerRight, node_.scale / 2.);
	topRightChild_ = new QuadTree(tempNode);
	topRightChild_->level = level + 1;
	topRightChild_->topNeighbour = &(*topNeighbour)->botRightChild_;
	topRightChild_->leftNeighbour = &topLeftChild_;
	topRightChild_->rightNeighbour = &(*rightNeighbour)->topLeftChild_;
	topRightChild_->botNeighbour = &botRightChild_;

	tempNode = QuadNode(node_.topLeft, node_.center, node_.scale / 2.);
	topLeftChild_ = new QuadTree(tempNode);
	topLeftChild_->level = level + 1;
	topLeftChild_->topNeighbour = &(*topNeighbour)->botLeftChild_;
	topLeftChild_->leftNeighbour = &(*leftNeighbour)->topRightChild_;
	topLeftChild_->rightNeighbour = &topRightChild_;
	topLeftChild_->botNeighbour = &botLeftChild_;

	glm::vec2 centerLeft = glm::vec2(node_.topLeft.x, node_.center.y);
	glm::vec2 botCenter = glm::vec2(node_.center.x, node_.botRight.y);
	tempNode = QuadNode(centerLeft, botCenter, node_.scale / 2.);
	botLeftChild_ = new QuadTree(tempNode);
	botLeftChild_->level = level + 1;
	botLeftChild_->topNeighbour = &topLeftChild_;
	botLeftChild_->leftNeighbour = &(*leftNeighbour)->botRightChild_;
	botLeftChild_->rightNeighbour = &botRightChild_;
	botLeftChild_->botNeighbour = &(*botNeighbour)->topLeftChild_;

	tempNode = QuadNode(node_.center, node_.botRight, node_.scale / 2.);
	botRightChild_ = new QuadTree(tempNode);
	botRightChild_->level = level + 1;
	botRightChild_->topNeighbour = &topRightChild_;
	botRightChild_->leftNeighbour = &botLeftChild_;
	botRightChild_->rightNeighbour = &(*rightNeighbour)->botLeftChild_;
	botRightChild_->botNeighbour = &(*botNeighbour)->topRightChild_;

	return true;
}


