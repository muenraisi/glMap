#include "point.h"

double Point::getX()
{
	return x_;
}

double Point::getY()
{
	return y_;
}

Point& Point::operator++()
{
	x_++;
	y_++;
	return *this;
}
Point Point::operator++(int)
{
	Point old = *this;
	++(*this);
	return old;
}
Point& Point:: operator --()
{
	x_--;
	y_--;
	return *this;
}
Point Point::operator --(int)
{
	Point old = *this;
	--(*this);
	return old;
}
Point Point::operator+(Point rhs)
{
	x_ += rhs.x_;
	y_ += rhs.y_;
	return *this;
}
Point Point::operator/(double rhs)
{
	x_ /= rhs;
	y_ /= rhs;
	return *this;
}
std::ostream& operator<<(std::ostream& out, Point& a)
{
	out << "(" << a.x_ << "," << a.y_ << ")";
	return out;

}