#pragma once
// C++ Implementation of Quad Tree 
#include <iostream> 
#include <vector>
#include <cmath> 
#include <glm/glm.hpp>

#include "Point.h"
#include "debug.h"
using namespace std;


/* 一个矩形区域的象限划分：:
TL(1)   |    TR(0)
--------|-----------
BL(2)   |    BR(3)
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
};


// The main quadtree class 
class QuadTree
{
public:
    int level;
    // Neighbour of the tree
    QuadTree** topNeighbour;
    QuadTree** leftNeighbour;
    QuadTree** botNeighbour;
    QuadTree** rightNeighbour;


    QuadTree();
    QuadTree(Point topL, Point botR, double scale);
    ~QuadTree();

    QuadNode* getNode() { return node_; };
    glm::vec3 getTrans();
    glm::vec4 getNeigh();

    void deleteSubtrees();

    void setChildren(QuadTree* topLeftChild, QuadTree* topRightChild,
        QuadTree* botLeftChild, QuadTree* botRightChild);
    std::vector<QuadTree*> getChildren();

    bool empty();
    bool split();

private:
    // Contains details of node 
    QuadNode* node_;

    // Children of the tree 
    QuadTree* topLeftChild_;
    QuadTree* topRightChild_;
    QuadTree* botLeftChild_;
    QuadTree* botRightChild_;


};
