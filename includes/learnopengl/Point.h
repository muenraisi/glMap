#pragma once
#include <ostream>


class Point
{
public:
	Point() :x_(0), y_(0) {}

	Point(double x, double y) :x_(x), y_(y) { }

	~Point() { }

	double getX();
	double getY();

	Point& operator ++();
	Point operator ++(int);
	Point& operator --();
	Point operator --(int);
	Point operator +(Point rhs);
	Point operator /(double rhs);
	friend std::ostream& operator <<(std::ostream& out, Point& a);
private:
	double x_, y_;
};
