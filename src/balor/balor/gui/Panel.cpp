#include "Panel.hpp"

#include <utility>

#include <balor/graphics/Brush.hpp>
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
static_assert(Panel::Options::transparent == WS_EX_TRANSPARENT, "Invalid enum value");

const int optionsMask = WS_EX_TRANSPARENT;
const int optionsXorMask = 0;
} // namespace


bool Panel::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



Panel::Panel() {
}


Panel::Panel(Panel&& value, bool checkSlicing)
	: ScrollableControl(move(value))
	, _onPaint(move(value._onPaint))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Panel::Panel(Control& parent, int x, int y, int width, int height, Control::Edge edge, Panel::Options options) {
	*this = fromParentHandle(parent, x, y, width, height, edge, options);
}


Panel::~Panel() {
}


Panel& Panel::operator=(Panel&& value) {
	if (&value != this) {
		this->~Panel();
		new (this) Panel(move(value));
	}
	return *this;
}


Control::Edge Panel::edge() const {
	return _handle.edge();
}


void Panel::edge(Control::Edge value) {
	_handle.edge(value);
}


bool Panel::focusable() const {
	return false; // 方向キーでフォーカスを得ないように上書き。
}


Panel Panel::fromParentHandle(HWND parent, int x, int y, int width, int height, Control::Edge edge, Panel::Options options) {
	assert("Invalid Panel::Options" && Options::_validate(options));
	int exStyle = options ^ optionsXorMask;
	int style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
	switch (edge) {
		case Edge::line   : style |= WS_BORDER; break;
		case Edge::sunken : exStyle |= WS_EX_STATICEDGE; break;
		case Edge::client : exStyle |= WS_EX_CLIENTEDGE; break;
	}
	Panel panel;
	panel.attachHandle(CreateWindowExW(exStyle, userClassName(), nullptr, style
		, x, y, width, height, parent, nullptr, nullptr, nullptr));
	panel._scrollSize = panel.clientSize();
	return panel;
}


Listener<Panel::Paint&>& Panel::onPaint() { return _onPaint; }


Panel::Options Panel::options() const {
	return static_cast<Options>((_handle.exStyle() & optionsMask) ^ optionsXorMask);
}


void Panel::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ERASEBKGND : {
			if (onPaint() || brush()) {
				msg.result = TRUE;
			} else {
				processMessageByDefault(msg);
			}
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
		default: {
			ScrollableControl::processMessage(msg);
		} break;
	}
}



	}
}