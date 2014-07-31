#include "LineLayout.hpp"

#include <algorithm>

#include <balor/gui/Scaler.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace gui {

using std::max;
using std::move;



LineLayout::LineLayout()
	: _vertical(false)
	, _startPosition(10, 10)
	, _margin(10, 10)
	{
}


LineLayout::LineLayout(LineLayout&& value)
	: _target(move(value._target))
	, _vertical(move(value._vertical))
	, _startPosition(move(value._startPosition))
	, _margin(move(value._margin))
	{
}


LineLayout::LineLayout(Control& target, bool vertical)
	: _target(&target)
	, _vertical(vertical)
	, _startPosition(10, 10)
	, _margin(10, 10)
	{
}


LineLayout::~LineLayout() {
}


LineLayout& LineLayout::operator=(LineLayout&& value) {
	if (this != &value) {
		_target = move(value._target);
		_vertical = move(value._vertical);
		_startPosition = move(value._startPosition);
		_margin = move(value._margin);
	}
	return *this;
}


Size LineLayout::margin() const {
	return _margin;
}


void LineLayout::margin(const Size& value) {
	_margin = value;
}


void LineLayout::margin(int width, int height) {
	margin(Size(width, height));
}


void LineLayout::perform() {
	assert("Null target" && _target);
	assert("target not created" && *_target);

	auto i = target()->controlsBegin();
	if (!i) {
		return;
	}
	const auto displayRect = target()->displayRectangle();
	const auto start = startPosition() + displayRect.position();
	i->position(start);
	auto lastBounds = i->bounds();
	if (vertical()) {
		int right = lastBounds.right();
		while (++i) {
			const int y = lastBounds.bottom() + margin().height;
			if (displayRect.height <  y + i->size().height) {
				i->position(right + margin().width, start.y);
			} else {
				i->position(lastBounds.x, y);
			}
			lastBounds = i->bounds();
			right = max(right, lastBounds.right());
		}
	} else {
		int bottom = lastBounds.bottom();
		while (++i) {
			const int x = lastBounds.right() + margin().width;
			if (displayRect.width <  x + i->size().width) {
				i->position(start.x, bottom + margin().height);
			} else {
				i->position(x, lastBounds.y);
			}
			lastBounds = i->bounds();
			bottom = max(bottom, lastBounds.bottom());
		}
	}
}


void LineLayout::scale(const Scaler& scaler) {
	startPosition(scaler.scale(startPosition()));
	margin(scaler.scale(margin()));
}


Point LineLayout::startPosition() const {
	return _startPosition;
}


void LineLayout::startPosition(const Point& value) {
	_startPosition = value;
}


void LineLayout::startPosition(int x, int y) {
	startPosition(Point(x, y));
}


Control* LineLayout::target() {
	return _target;
}


bool LineLayout::vertical() const {
	return _vertical;
}


void LineLayout::vertical(bool value) {
	_vertical = value;
}



	}
}