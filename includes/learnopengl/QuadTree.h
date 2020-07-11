#pragma once
// C++ Implementation of Quad Tree 
#include <iostream> 
#include <vector>
#include <cmath> 
#include <glm/glm.hpp>

#include "Point.h"
using namespace std;


/* 一个矩形区域的象限划分：:
UL(1)   |    UR(0)
--------|-----------
LL(2)   |    LR(3)
以下对该象限类型的枚举
*/

// 目的: 划分好节点，并生成网格



// The objects that we want stored in the quadtree 
struct QuadNode
{
    Point center;
    Point topLeft;
    Point botRight;
    double scale=1.;
    QuadNode(Point _topLeft, Point _botRight, double _scale=1.0)
    {
        assert(_topLeft.GetX() <= _botRight.GetX());
        assert(_topLeft.GetY() >= _botRight.GetY());
        topLeft = _topLeft;
        botRight = _botRight;
        center = (_topLeft+_botRight)/2.;
        scale = _scale;
    }
    QuadNode()
    {
    }
};

// The main quadtree class 
class QuadTree
{
public:
    QuadTree();
    QuadTree(Point topL, Point botR, double scale);

    QuadNode* getNode() { return quadNode_; };
    glm::vec3 getTrans();
    std::vector<QuadTree*>* getTrees();

    bool empty();
    void split();

private:
    // Contains details of node 
    QuadNode* quadNode_;

    // Children of this tree 
    QuadTree* topLeftTree_;
    QuadTree* topRightTree_;
    QuadTree* botLeftTree_;
    QuadTree* botRightTree_;

};
