#include <assert.h>

#include "QuadTree.h"

QuadTree::QuadTree()
{

	quadNode_ = nullptr;
	topLeftTree_ = nullptr;
	topRightTree_ = nullptr;
	botLeftTree_ = nullptr;
	botRightTree_ = nullptr;


}



QuadTree::QuadTree(Point topL, Point botR, double scale = 1.0)
{
	quadNode_ = new QuadNode(topL, botR, scale);
	topLeftTree_ = nullptr;
	topRightTree_ = nullptr;
	botLeftTree_ = nullptr;
	botRightTree_ = nullptr;
}

glm::vec3 QuadTree::getTrans()
{
	return glm::vec3(quadNode_->center.GetX(),quadNode_->center.GetY(),quadNode_->scale);
}

std::vector<QuadTree*>* QuadTree::getTrees()
{
	return new std::vector<QuadTree*>{ topLeftTree_, topRightTree_, botLeftTree_, botRightTree_ };
}

bool QuadTree::empty()
{
	if (topLeftTree_ == nullptr || topRightTree_==nullptr || botLeftTree_ == nullptr || botRightTree_==nullptr)
		return true;
	else
		return false;
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

	topLeftTree_ = new QuadTree(quadNode_->topLeft, quadNode_->center, quadNode_->scale / 2.);
	Point topCenter = Point(quadNode_->center.GetX(), quadNode_->topLeft.GetY());
	Point centerRight = Point(quadNode_->botRight.GetX(), quadNode_->center.GetY());
	topRightTree_ = new QuadTree(topCenter, centerRight, quadNode_->scale / 2.);
	Point centerLeft = Point(quadNode_->topLeft.GetX(), quadNode_->center.GetY());
	Point botCenter = Point(quadNode_->center.GetX(), quadNode_->botRight.GetY());
	botLeftTree_ = new QuadTree(centerLeft, botCenter, quadNode_->scale / 2.);
	botRightTree_ = new QuadTree(quadNode_->center, quadNode_->botRight, quadNode_->scale / 2.);
}
