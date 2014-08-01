#include "Padding.hpp"

#include <balor/Size.hpp>


namespace balor {
	namespace gui {



Padding::Padding(const Padding& value) : left(value.left), top(value.top), right(value.right), bottom(value.bottom) {
}


Padding::Padding(int all) : left(all), top(all), right(all), bottom(all) {
}


Padding::Padding(int left, int top, int right, int bottom) : left(left), top(top), right(right), bottom(bottom) {
}


int Padding::horizontal() const {
	return left + right;
}


Size Padding::size() const {
	return Size(horizontal(), vertical());
}


int Padding::vertical() const {
	return top + bottom;
}


Padding& Padding::operator += (const Padding& rhs) {
	left   += rhs.left;
	top    += rhs.top;
	right  += rhs.right;
	bottom += rhs.bottom;
	return *this;
}


Padding operator + (const Padding& lhs, const Padding& rhs) {
	return Padding(lhs.left + rhs.left, lhs.top + rhs.top, lhs.right + rhs.right, lhs.bottom + rhs.bottom);
}


Padding& Padding::operator -= (const Padding& rhs) {
	left   -= rhs.left;
	top    -= rhs.top;
	right  -= rhs.right;
	bottom -= rhs.bottom;
	return *this;
}


Padding operator - (const Padding& lhs, const Padding& rhs) {
	return Padding(lhs.left - rhs.left, lhs.top - rhs.top, lhs.right - rhs.right, lhs.bottom - rhs.bottom);
}


bool Padding::operator == (const Padding& rhs) const {
	return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
}


bool Padding::operator != (const Padding& rhs) const {
	return !(*this == rhs);
}



	}
}