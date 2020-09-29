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
}


QuadTree::QuadTree(Rect& _rect, int _level)
{
	rect = _rect;
	level = _level;

	topNeighbour = nullptr;
	leftNeighbour = nullptr;
	botNeighbour = nullptr;
	rightNeighbour = nullptr;

	topLeftChild_ = nullptr;
	topRightChild_ = nullptr;
	botLeftChild_ = nullptr;
	botRightChild_ = nullptr;
}

QuadTree::~QuadTree()
{
	deleteSubtrees();
}


glm::vec3 QuadTree::getTrans()
{
	return glm::vec3(rect.x, rect.y, pow(2, -level));
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
	Rect childRect(0.,0.,rect.w/2., rect.h/2.);
	childRect.setCenter(rect.x+rect.w/4., rect.y+rect.h/4.);
	topRightChild_ = new QuadTree(childRect, level +1);
	topRightChild_->topNeighbour = &(*topNeighbour)->botRightChild_;
	topRightChild_->leftNeighbour = &topLeftChild_;
	topRightChild_->rightNeighbour = &(*rightNeighbour)->topLeftChild_;
	topRightChild_->botNeighbour = &botRightChild_;

	childRect.setCenter(rect.x - rect.w / 4., rect.y + rect.h / 4.);
	topLeftChild_ = new QuadTree(childRect, level +1);
	topLeftChild_->topNeighbour = &(*topNeighbour)->botLeftChild_;
	topLeftChild_->leftNeighbour = &(*leftNeighbour)->topRightChild_;
	topLeftChild_->rightNeighbour = &topRightChild_;
	topLeftChild_->botNeighbour = &botLeftChild_;

	childRect.setCenter(rect.x - rect.w / 4., rect.y -rect.h / 4.);
	botLeftChild_ = new QuadTree(childRect, level + 1);
	botLeftChild_->topNeighbour = &topLeftChild_;
	botLeftChild_->leftNeighbour = &(*leftNeighbour)->botRightChild_;
	botLeftChild_->rightNeighbour = &botRightChild_;
	botLeftChild_->botNeighbour = &(*botNeighbour)->topLeftChild_;

	childRect.setCenter(rect.x + rect.w / 4., rect.y - rect.h / 4.);
	botRightChild_ = new QuadTree(childRect, level + 1);
	botRightChild_->topNeighbour = &topRightChild_;
	botRightChild_->leftNeighbour = &botLeftChild_;
	botRightChild_->rightNeighbour = &(*rightNeighbour)->botLeftChild_;
	botRightChild_->botNeighbour = &(*botNeighbour)->topRightChild_;

	return true;
}


