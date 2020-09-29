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

// The objects that we want stored in the quadtree 
struct Rect
{
  double x;
  double y;
  double w;
  double h;

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
#ifdef DEBUG
    assert(_x >= 0);
    assert(_y >= 0);
#endif // DEBUG
    x = _x;
    y = _y;
  }
  void setSize(double _w, double _h) {
#ifdef DEBUG
    assert(_w >= 0);
    assert(_h >= 0);
#endif // DEBUG
    w = _w;
    h = _h;
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

    Rect rect; 

    glm::vec4 realHeight; // the exact height based on the position 
    glm::vec4 imageHeight; // interplot height of the mothers boudnary

    

    QuadTree();
    QuadTree(Rect& _rect, int _level=0);
    ~QuadTree();

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
    // Children of the tree 
    QuadTree* topRightChild_;
    QuadTree* topLeftChild_;
    QuadTree* botLeftChild_;
    QuadTree* botRightChild_;


};
