#pragma once
// C++ Implementation of Quad Tree 
#include <iostream> 
#include <vector>
#include <queue>
#include <cmath> 
#include <glm/glm.hpp>

#include "debug.h"
#include "heightMap.h"
using namespace std;


/* 一个矩形区域的象限划分：:
TL(1)   |    TR(0)
--------|-----------
BL(2)   |    BR(3)
以下对该象限类型的枚举
*/

// The objects that we want stored in the quadtree 
struct Rect
{
  double x,y,w,h;

  Rect() {
    x = y = w = h = 0;
  }

  Rect(double _x, double _y, double _w, double _h)
  {
    setCenter(_x, _y);
    setSize(_w, _h);
  }
  bool isEmpty() {
    if (x == 0 && y == 0 && w == 0 && h == 0)
      return true;
    else
      return false;
  }
  void setCenter(double _x, double _y) {
    assert(_x >= 0 && _y>=0);
    x = _x;
    y = _y;
  }
  void setSize(double _w, double _h) {
    assert(_w >= 0 && _h>=0);
    w = _w;
    h = _h;
  }
};

// The main quadtree class 
class QuadTree
{
public:

    // Neighbour of the tree, MUST use double pointer to avoid 
    // refering an empty position when split tree
    QuadTree** topNeighbour;
    QuadTree** leftNeighbour;
    QuadTree** botNeighbour;
    QuadTree** rightNeighbour;

    QuadTree* parentTree;

    Rect rect;   

    QuadTree();
    QuadTree(Rect& _rect, int _level=0);
    ~QuadTree();

    int getLevel();
    glm::vec3 getTrans();
    glm::vec4 getNeighbour();
    glm::vec4 getHeight( HeightMap &heightMap, float scale);
    std::vector<glm::vec2> getConers();

    
    void deleteSubtrees();

    void setChildren(QuadTree* topRightChild, QuadTree* topLeftChild,
        QuadTree* botLeftChild, QuadTree* botRightChild);
    std::vector<QuadTree*> getChildren();
    std::vector<QuadTree*> getDescendants();

    bool isLeaf();
    bool split();

private:
    int level_;

    // Children of the tree 
    QuadTree* topRightChild_;
    QuadTree* topLeftChild_;
    QuadTree* botLeftChild_;
    QuadTree* botRightChild_;


};
