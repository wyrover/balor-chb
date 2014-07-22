#include "Rectangle.hpp"

#include <algorithm>

#include <balor/system/windows.hpp>
#include <balor/Point.hpp>
#include <balor/Size.hpp>


namespace balor {


using std::min;
using std::max;



Rectangle::Rectangle(const Rectangle& rectangle)
	: x(rectangle.x)
	, y(rectangle.y)
	, width (rectangle.width )
	, height(rectangle.height) {
}


Rectangle::Rectangle(const RECT& rect)
	: x(rect.left)
	, y(rect.top)
	, width (rect.right  - rect.left)
	, height(rect.bottom - rect.top ) {
}


Rectangle::Rectangle(const Point& position, const Size& size)
	: x(position.x)
	, y(position.y)
	, width (size.width )
	, height(size.height) {
}


Rectangle::Rectangle(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {
}


Point Rectangle::bottomLeft() const {
	return Point(x, y + height);
}


Point Rectangle::bottomRight() const {
	return Point(x + width, y + height);
}


bool Rectangle::contains(const Point& point) const {
	return contains(point.x, point.y);
}


bool Rectangle::contains(int x, int y) const {
	return left() <= x && x < right()
		&& top()  <= y && y < bottom();
}


bool Rectangle::contains(const Rectangle& rect) const {
	return left() <= rect.left() && rect.right() <= right()
		&& top()  <= rect.top()  && rect.bottom() <= bottom();
}


Rectangle Rectangle::fromLTRB(int left, int top, int right, int bottom) {
	return Rectangle(left, top, right - left, bottom - top);
}


void Rectangle::grow(const Size& size) {
	grow(size.width, size.height);
}


void Rectangle::grow(int width, int height) {
	x -= width;
	y -= height;
	this->width  += width  * 2;
	this->height += height * 2;
}


Rectangle Rectangle::intersection(const Rectangle& lhs, const Rectangle& rhs) {
	const Rectangle result =  fromLTRB(max(lhs.x, rhs.x)
									  ,max(lhs.y, rhs.y)
									  ,min(lhs.right (), rhs.right ())
									  ,min(lhs.bottom(), rhs.bottom()));
	if (result.width < 0 || result.height < 0) {
		return Rectangle(0, 0, 0, 0);
	}
	return result;
}


bool Rectangle::intersects(const Rectangle& rhs) const {
	return x < rhs.right()  && rhs.x < right()
		&& y < rhs.bottom() && rhs.y < bottom();
}


Rectangle Rectangle::or(const Rectangle& lhs, const Rectangle& rhs) {
	return fromLTRB(min(lhs.x, rhs.x)
				   ,min(lhs.y, rhs.y)
				   ,max(lhs.right() , rhs.right() )
				   ,max(lhs.bottom(), rhs.bottom()));
}


Point Rectangle::position() const {
	return Point(x, y);
}


void Rectangle::position(const Point& value) {
	x = value.x;
	y = value.y;
}


void Rectangle::position(int x, int y) {
	this->x = x;
	this->y = y;
}


Size Rectangle::size() const {
	return Size(width, height);
}


void Rectangle::size(const Size& value) {
	width  = value.width;
	height = value.height;
}


void Rectangle::size(int width, int height) {
	this->width  = width;
	this->height = height;
}


Point Rectangle::topLeft() const {
	return Point(x, y);
}


Point Rectangle::topRight() const {
	return Point(x + width, y);
}


void Rectangle::translate(const Point& point) {
	translate(point.x, point.y);
}


void Rectangle::translate(int x, int y) {
	this->x += x;
	this->y += y;
}


Rectangle::operator RECT() const {
	const RECT rect = {left(), top(), right(), bottom()};
	return rect;
}


bool Rectangle::operator == (const Rectangle& rhs) const {
	return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
}


bool Rectangle::operator != (const Rectangle& rhs) const {
	return !(*this == rhs);
}



}