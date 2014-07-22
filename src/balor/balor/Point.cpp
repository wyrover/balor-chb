#include "Point.hpp"

#include <algorithm>

#include <balor/system/windows.hpp>


namespace balor {


using std::max;
using std::min;



Point::Point(const Point& value) : x(value.x), y(value.y) {
}


Point::Point(const POINT& point) : x(point.x), y(point.y) {
}


Point::Point(int x, int y) : x(x), y(y) {
}


Point Point::maximize(const Point& lhs, const Point& rhs) {
	return Point(max(lhs.x, rhs.x)
			   , max(lhs.y, rhs.y));
}


Point Point::minimize(const Point& lhs, const Point& rhs) {
	return Point(min(lhs.x, rhs.x)
			   , min(lhs.y, rhs.y));
}


Point::operator POINT() const {
	const POINT point = {x, y};
	return point;
}


Point Point::operator-() const {
	return Point(-x, -y);
}


Point& Point::operator += (const Point& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}


Point operator + (const Point& lhs, const Point& rhs) {
	return Point(lhs.x + rhs.x
			   , lhs.y + rhs.y);
}


Point& Point::operator -= (const Point& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}


Point operator - (const Point& lhs, const Point& rhs) {
	return Point(lhs.x - rhs.x
			   , lhs.y - rhs.y);
}


bool Point::operator == (const Point& rhs) const {
	return x == rhs.x
		&& y == rhs.y;
}


bool Point::operator != (const Point& rhs) const {
	return !(*this == rhs);
}



}