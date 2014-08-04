#include "Splitter.hpp"

#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Cursor.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>


namespace balor {
	namespace gui {

using std::move;
using namespace balor::graphics;


namespace {
} // namespace



Splitter::Splitter()
	: _movable(false)
	, _zOrderable(false)
	, _margin(0)
	{
}


Splitter::Splitter(Splitter&& value, bool checkSlicing)
	: Control(move(value))
	, _movable(move(value._movable))
	, _zOrderable(move(value._zOrderable))
	, _margin(move(value._margin))
	, _onPaint(move(value._onPaint))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Splitter::Splitter(Control& parent, int x, int y, int width, int height, Control::Edge edge)
	: _movable(false)
	, _zOrderable(false)
	, _margin(0)
	{
	int exStyle = 0;
	int style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
	switch (edge) {
		case Edge::line   : style |= WS_BORDER; break;
		case Edge::sunken : exStyle |= WS_EX_STATICEDGE; break;
		case Edge::client : exStyle |= WS_EX_CLIENTEDGE; break;
	}
	attachHandle(CreateWindowExW(exStyle, userClassName(), nullptr, style
		, x, y, width, height, parent, nullptr, nullptr, nullptr));
}


Splitter::~Splitter() {
}


Splitter& Splitter::operator=(Splitter&& value) {
	if (&value != this) {
		this->~Splitter();
		new (this) Splitter(move(value));
	}
	return *this;
}


void Splitter::bounds(const Rectangle& value) {
	_movable = true;
	scopeExit([&] () {
		_movable = false;
	});
	Control::bounds(value);
}


Control::Edge Splitter::edge() const {
	return _handle.edge();
}


void Splitter::edge(Control::Edge value) {
	_handle.edge(value);
}


bool Splitter::focusable() const {
	return false; // 方向キーでフォーカスを得ないように上書き
}


Padding Splitter::margin() const {
	return _margin;
}


void Splitter::margin(const Padding& value) {
	_margin = value;
}


Listener<Splitter::Paint&>& Splitter::onPaint() { return _onPaint; }


void Splitter::position(const Point& value) {
	_movable = true;
	scopeExit([&] () {
		_movable = false;
	});
	Control::position(value);
}


bool Splitter::vertical() const {
	auto size = this->size();
	return size.width < size.height;
}


void Splitter::zOrder(int value) {
	_zOrderable = true;
	scopeExit([&] () {
		_zOrderable = false;
	});
	Control::zOrder(value);
}


void Splitter::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ERASEBKGND : {
			if (onPaint() || brush()) {
				msg.result = TRUE;
			} else {
				processMessageByDefault(msg);
			}
		} break;
		case WM_NCHITTEST : {
			msg.result = HTCAPTION; // 全体をタイトルバーとみなしてドラッグ移動できるようにする
		} break;
		case WM_PAINT : {
			PAINTSTRUCT paint;
			Graphics graphics(BeginPaint(handle(), &paint));
			scopeExit([&] () {
				EndPaint(handle(), &paint);
			});
			if (onPaint()) {
				Paint event(*this, graphics);
				onPaint()(event);
			} else {
				auto brush = this->brush();
				if (brush) {
					auto displayRect = displayRectangle();
					const auto origin = graphics.brushOrigin(graphics.brushOrigin() + displayRect.position() + brushOrigin());
					brush = graphics.brush(brush);
					displayRect.grow(1, 1);
					graphics.drawRectangle(displayRect);
					graphics.brushOrigin(origin);
					graphics.brush(brush);
				} 
			}
		} break;
		case WM_PRINTCLIENT : {
			if (onPaint()) {
				Paint event(*this, (HDC)msg.wparam);
				onPaint()(event);
			}
			processMessageByDefault(msg);
		} break;
		case WM_SETCURSOR : {
			Mouse::cursor(vertical() ? Cursor::sizeWE() : Cursor::sizeNS());
		} break;
		case WM_WINDOWPOSCHANGING : {
			auto info = (WINDOWPOS*)msg.lparam;
			if (!_zOrderable) { // ドラッグすると最前面に移動するのを阻止する。DockLayout でレイアウトが変化してしまうので
				info->flags |= SWP_NOZORDER;
			}
			if (!toFlag(info->flags)[SWP_NOMOVE] && !_movable) {
				auto bounds = this->bounds();
				Point newPosition(info->x, info->y);
				if (newPosition != bounds.position()) {
					auto vertical = this->vertical();
					// 移動を垂直または水平に制限する
					if (vertical) {
						newPosition.y = bounds.y;
					} else {
						newPosition.x = bounds.x;
					}
					auto diff = newPosition - bounds.position();
					auto parent = this->parent();
					assert(parent);
					// 移動位置が限界からはみでるのを阻止する
					for (auto i = parent->controlsBegin(); i; ++i) {
						auto childBounds = i->bounds();
						if (vertical) {
							if (bounds.top() <= childBounds.top() && childBounds.bottom() <= bounds.bottom()) {
								// 左に接するかどうか
								if (childBounds.right() == bounds.left()) {
									// 左にはみでるかどうか
									if (childBounds.width  + diff.x < _margin.left ) {
										newPosition.x = childBounds.left() + _margin.left;
										diff.x = newPosition.x - bounds.x;
									}
								}
								// 右に接するかどうか
								if (childBounds.left() == bounds.right()) {
									// 右にはみでるかどうか
									if (childBounds.width  - diff.x < _margin.right) {
										newPosition.x = childBounds.right() - _margin.right - bounds.width;
										diff.x = newPosition.x - bounds.x;
									}
								}
							}
						} else {
							if (bounds.left() <= childBounds.left() && childBounds.right() <= bounds.right()) {
								// 上に接するかどうか
								if (childBounds.bottom() == bounds.top()) {
									// 上にはみでるかどうか
									if (childBounds.height + diff.y < _margin.top  ) {
										newPosition.y = childBounds.top() + _margin.top;
										diff.y = newPosition.y - bounds.y;
									}
								}
								// 下に接するかどうか
								if (childBounds.top() == bounds.bottom()) {
									// 下にはみでるかどうか
									if (childBounds.height - diff.y < _margin.bottom) {
										newPosition.y = childBounds.bottom() - _margin.bottom - bounds.height;
										diff.y = newPosition.y - bounds.y;
									}
								}
							}
						}
					}
					// 接しているコントロールの大きさを変える
					for (auto i = parent->controlsBegin(); i; ++i) {
						auto childBounds = i->bounds();
						if (vertical) {
							if (bounds.top() <= childBounds.top() && childBounds.bottom() <= bounds.bottom()) {
								// 左に接するかどうか
								if (childBounds.right() == bounds.left()) {
									childBounds.width  += diff.x;
									i->bounds(childBounds);
								}
								// 右に接するかどうか
								if (childBounds.left() == bounds.right()) {
									childBounds.x += diff.x;
									childBounds.width  -= diff.x;
									i->bounds(childBounds);
								}
							}
						} else {
							if (bounds.left() <= childBounds.left() && childBounds.right() <= bounds.right()) {
								// 上に接するかどうか
								if (childBounds.bottom() == bounds.top()) {
									childBounds.height += diff.y;
									i->bounds(childBounds);
								}
								// 下に接するかどうか
								if (childBounds.top() == bounds.bottom()) {
									childBounds.y += diff.y;
									childBounds.height -= diff.y;
									i->bounds(childBounds);
								}
							}
						}
					}
					// 最終的な移動先を設定
					info->x = newPosition.x;
					info->y = newPosition.y;
				}
			}
			Control::processMessage(msg);
		} break;
		default: {
			Control::processMessage(msg);
		} break;
	}
}



	}
}