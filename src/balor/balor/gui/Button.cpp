#include "Button.hpp"

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Scaler.hpp>
#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::move;
using namespace balor::graphics;



namespace {
static_assert(ButtonBase::TextAlign::topLeft      == (BS_TOP     | BS_LEFT  ), "Invalid enum value");
static_assert(ButtonBase::TextAlign::topCenter    == (BS_TOP     | BS_CENTER), "Invalid enum value");
static_assert(ButtonBase::TextAlign::topRight     == (BS_TOP     | BS_RIGHT ), "Invalid enum value");
static_assert(ButtonBase::TextAlign::middleLeft   == (BS_VCENTER | BS_LEFT  ), "Invalid enum value");
static_assert(ButtonBase::TextAlign::middleCenter == (BS_VCENTER | BS_CENTER), "Invalid enum value");
static_assert(ButtonBase::TextAlign::middleRight  == (BS_VCENTER | BS_RIGHT ), "Invalid enum value");
static_assert(ButtonBase::TextAlign::bottomLeft   == (BS_BOTTOM  | BS_LEFT  ), "Invalid enum value");
static_assert(ButtonBase::TextAlign::bottomCenter == (BS_BOTTOM  | BS_CENTER), "Invalid enum value");
static_assert(ButtonBase::TextAlign::bottomRight  == (BS_BOTTOM  | BS_RIGHT ), "Invalid enum value");

const int textAlignMask = BS_TOP | BS_VCENTER | BS_BOTTOM | BS_LEFT | BS_CENTER | BS_RIGHT;
} // namespace



bool ButtonBase::TextAlign::_validate(TextAlign value) {
	return !(value & ~textAlignMask);
}



ButtonBase::ButtonBase() {
}


ButtonBase::ButtonBase(ButtonBase&& value)
	: Control(move(value))
	{
}


ButtonBase::~ButtonBase() {
}


bool ButtonBase::multiline() const {
	return _handle.hasStyle(BS_MULTILINE);
}


void ButtonBase::multiline(bool value) {
	_handle.setStyle(BS_MULTILINE, value);
}


bool ButtonBase::pushed() const {
	auto state = SendMessageW(handle(), BM_GETSTATE, 0, 0);
	return toFlag(state)[BST_PUSHED];
}


ButtonBase::TextAlign ButtonBase::textAlign() const {
	return static_cast<TextAlign>(_handle.style() & textAlignMask);
}


void ButtonBase::textAlign(ButtonBase::TextAlign value) {
	assert("Invalid ButtonBase::TextAlign" && TextAlign::_validate(value));
	if (value != textAlign()) {
		_handle.style(toFlag(_handle.style()).set(textAlignMask, false).set(value, true));
		_handle.updateStyle();
	}
}



Button::Button() {
}


Button::Button(Button&& value, bool checkSlicing)
	: ButtonBase(move(value))
	, _onClick(move(value._onClick))
	, _onPaint(move(value._onPaint))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Button::Button(Control& parent, int x, int y, int width, int height, StringRange text, Listener<Button::Event&> onClick) : _onClick(onClick) {
	attachHandle(CreateWindowExW(0, L"BUTTON", text.c_str()
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | TextAlign::middleCenter
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	size(getPreferredSize(width, height));
}


Button::~Button() {
}


Button& Button::operator=(Button&& value) {
	if (&value != this) {
		this->~Button();
		new (this) Button(move(value));
	}
	return *this;
}


bool Button::defaultButton() const {
	return _handle.hasStyle(BS_DEFPUSHBUTTON);
}


void Button::defaultButton(bool value) {
	_handle.setStyle(BS_DEFPUSHBUTTON, value);
}


Size Button::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	SIZE size = {width, height};
	if (!SendMessageW(handle(), BCM_GETIDEALSIZE, 0, (LPARAM)&size)) {
		Graphics graphics(*this);
		auto oldFont = graphics.font(font());
		auto measureSize = graphics.measureText(text());
		size.cx = measureSize.width;
		size.cy = measureSize.height;
	}
	Scaler scaler;
	size.cx += scaler.scale(14);
	size.cy += scaler.scale( 9);
	return Size(width  ? width  : size.cx
			   ,height ? height : size.cy);
}


Listener<Button::Click&>& Button::onClick() { return _onClick; }
Listener<Button::Paint&>& Button::onPaint() { return _onPaint; }


void Button::performClick() {
	if (focusable()) {
		Event event(*this);
		onClick()(event);
	}
}


void Button::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ERASEBKGND : {
			if (!onPaint()) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_KILLFOCUS : {
			Control::processMessage(msg);
			if (!findFrame()) { // ボタンはフォーカスを得ると自動的に defaultButton() が true になるので解除が必要。Frame がなければ自分でやるしかない。
				defaultButton(false);
			}
		} break;
		case WM_MOVE : {
			ButtonBase::processMessage(msg);
			auto parent = this->parent();
			if (parent && parent->brush() && !onPaint()) { // 親コントロールと地続きな背景を持つので描画しなおし
				invalidate();
			}
		} break;
		case WM_PAINT : {
			if (onPaint()) {
				PAINTSTRUCT paint;
				Paint event(*this, BeginPaint(handle(), &paint));
				scopeExit([&] () {
					EndPaint(handle(), &paint);
				});
				onPaint()(event);
			} else {
				processMessageByDefault(msg);
			}
		} break;
		case WM_PRINTCLIENT : {
			if (onPaint()) {
				Paint event(*this, (HDC)msg.wparam);
				onPaint()(event);
			} else {
				processMessageByDefault(msg);
			}
		} break;
		case wmReflect + WM_COMMAND : {
			switch (HIWORD(msg.wparam)) {
				case BN_CLICKED : {
					Click event(*this);
					onClick()(event);
				} break;
			}
			ButtonBase::processMessage(msg);
		} break;
		case wmReflect + WM_CTLCOLORBTN : {
			if (!onPaint()) {
				HBRUSH brush = this->brush();
				auto origin = brushOrigin();
				if (!brush) { // 親コントロールと地続きな背景を持つので親のブラシがあれば使う
					auto parent = this->parent();
					if (parent) {
						brush = parent->brush();
						origin = parent->displayRectangle().position() + parent->brushOrigin() - position();
					}
				}
				if (brush) {
					Graphics graphics((HDC)msg.wparam);
					graphics.brushOrigin(origin);
					msg.result = (LRESULT)brush;
					return;
				}
			}
			ButtonBase::processMessage(msg);
		} break;
		case wmReflect + WM_NOTIFY : {
			if (onPaint()) {
				if (((NMHDR*)msg.lparam)->code == NM_CUSTOMDRAW) {
					auto info = (NMCUSTOMDRAW*)msg.lparam;
					if (info->dwDrawStage == CDDS_PREPAINT || info->dwDrawStage == CDDS_PREERASE) {
						invalidate();
						msg.result = CDRF_SKIPDEFAULT; // WM_PAINT 以外からも勝手に描画されることがあるのでやめさせる。
						return;
					}
				}
			}
			ButtonBase::processMessage(msg);
		} break;
		default : {
			ButtonBase::processMessage(msg);
		} break;
	}
}



	}
}