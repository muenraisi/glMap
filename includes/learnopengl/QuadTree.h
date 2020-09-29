#pragma once
// C++ Implementation of Quad Tree 
#include <iostream> 
#include <vector>
#include <cmath> 
#include <glm/glm.hpp>

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
    glm::vec2 center;
    glm::vec2 topLeft;
    glm::vec2 botRight;

    double scale;
    QuadNode()
    {
      center = glm::vec2();
      topLeft = glm::vec2();
      botRight = glm::vec2();
      scale = 1.;
    }
    QuadNode( glm::vec2 _topLeft,  glm::vec2 _botRight,  double _scale=1.0)
    {
        assert(_topLeft.x <= _botRight.x);
        assert(_topLeft.y >= _botRight.y);
        topLeft = _topLeft;
        botRight = _botRight;
        // default and will be change if close to camera
        center = (_topLeft+_botRight)/2.f;
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

    glm::vec4 realHeight; // the exact height based on the position 
    glm::vec4 imageHeight; // interplot height of the mothers boudnary


    QuadTree();
    QuadTree(QuadNode& quadNode);
    ~QuadTree();

    QuadNode* getNode();
    glm::vec3 getTrans();
    glm::vec4 getNeighbour();
    glm::vec4 getHeight();

    void deleteSubtrees();

    void setChildren(QuadTree* topRightChild, QuadTree* topLeftChild,
        QuadTree* botLeftChild, QuadTree* botRightChild);
    std::vector<QuadTree*> getChildren();

    bool empty();
    bool split();

private:
    // Contains details of node 
    QuadNode node_;

    // Children of the tree 
    QuadTree* topRightChild_;
    QuadTree* topLeftChild_;
    QuadTree* botLeftChild_;
    QuadTree* botRightChild_;


};
