#include "Size.hpp"

#include <algorithm>

#include <balor/Point.hpp>


namespace balor {


using std::max;
using std::min;



Size::Size(const Size& size)
	: width (size.width )
	, height(size.height) {
}


Size::Size(const Point& point)
	: width (point.x)
	, height(point.y) {
}


Size::Size(int width, int height)
	: width (width )
	, height(height) {
}


Size Size::minimize(const Size& lhs, const Size& rhs) {
	return Size(min(lhs.width , rhs.width )
			  , min(lhs.height, rhs.height));
}


Size Size::maximize(const Size& lhs, const Size& rhs) {
	return Size(max(lhs.width , rhs.width )
			  , max(lhs.height, rhs.height));
}


bool Size::negative() const {
	return width  < 0
		|| height < 0;
}


Size::operator Point() const {
	return Point(width, height);
}


Size& Size::operator += (const Size& rhs) {
	width  += rhs.width;
	height += rhs.height;
	return *this;
}


Size operator + (const Size& lhs, const Size& rhs) {
	return Size(lhs.width  + rhs.width
			  , lhs.height + rhs.height);
}


Size& Size::operator -= (const Size& rhs) {
	width  -= rhs.width;
	height -= rhs.height;
	return *this;
}


Size operator - (const Size& lhs, const Size& rhs) {
	return Size(lhs.width  - rhs.width
			  , lhs.height - rhs.height);
}


Size& Size::operator *= (int rhs) {
	width  *= rhs;
	height *= rhs;
	return *this;
}


Size operator * (const Size& lhs, int rhs) {
	return Size(lhs.width  * rhs
			  , lhs.height * rhs);
}


Size& Size::operator /= (int rhs) {
	width  /= rhs;
	height /= rhs;
	return *this;
}


Size operator / (const Size& lhs, int rhs) {
	return Size(lhs.width  / rhs
			  , lhs.height / rhs);
}


bool Size::operator == (const Size& rhs) const {
	return width  == rhs.width
		&& height == rhs.height;
}


bool Size::operator != (const Size& rhs) const {
	return !(*this == rhs);
}



}