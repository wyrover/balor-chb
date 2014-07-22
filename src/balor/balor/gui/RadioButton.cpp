#include "RadioButton.hpp"

#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Mouse.hpp>
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



RadioButton::RadioButton() : _checkedChanging(false) {
}


RadioButton::RadioButton(RadioButton&& value, bool checkSlicing)
	: ButtonBase(move(value))
	, _checkedChanging(move(value._checkedChanging))
	, _onCheck(move(value._onCheck))
	, _onPaint(move(value._onPaint))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


RadioButton::RadioButton(Control& parent, int x, int y, int width, int height, StringRange text, bool firstOfGroup) : _checkedChanging(false) {
	attachHandle(CreateWindowExW(0, L"BUTTON", text.c_str()
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | BS_AUTORADIOBUTTON | TextAlign::middleLeft | (firstOfGroup ? WS_GROUP : 0)
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	size(getPreferredSize(width, height));
}


RadioButton::RadioButton(Control& parent, int x, int y, int width, int height, StringRange text, Listener<RadioButton::Check&> onCheck, bool firstOfGroup)
	: _checkedChanging(false) {
	*this = RadioButton(parent, x, y, width, height, text, firstOfGroup);
	_onCheck = move(onCheck);
}



RadioButton::~RadioButton() {
}


RadioButton& RadioButton::operator=(RadioButton&& value) {
	if (&value != this) {
		this->~RadioButton();
		new (this) RadioButton(move(value));
	}
	return *this;
}


bool RadioButton::autoCheck() const {
	return _handle.hasStyle(BS_AUTORADIOBUTTON);
}


void RadioButton::autoCheck(bool value) {
	if (value != autoCheck()) {
		auto style = toFlag(_handle.style()).set(BS_RADIOBUTTON | BS_AUTORADIOBUTTON, false);
		style.set(value ? BS_AUTORADIOBUTTON : BS_RADIOBUTTON, true);
		_handle.style(style);
		_handle.updateStyle();
	}
}


bool RadioButton::checked() const {
	return SendMessageW(handle(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}


void RadioButton::checked(bool value) {
	if (value != checked()) {
		if (value) {
			SendMessageW(handle(), BM_CLICK, 0, 0);
		} else {
			SendMessageW(handle(), BM_SETCHECK, BST_UNCHECKED, 0);
		}
	}
}


Size RadioButton::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	SIZE size = {width, height};
	Scaler scaler;
	//if (!SendMessageW(handle(), BCM_GETIDEALSIZE, 0, (LPARAM)&size)) { // XP でうまく動作せず
		Graphics graphics(*this);
		auto oldFont = graphics.font(font());
		auto measureSize = graphics.measureText(text());
		size.cx = measureSize.width;
		size.cy = measureSize.height;
		if (!pushLike()) {
			size.cx += scaler.scale(25);
			size.cy += scaler.scale( 5);
		}
	//}
	if (pushLike()) {
		size.cx += scaler.scale(14);
		size.cy += scaler.scale( 9);
	}
	return Size(width  ? width  : size.cx
			   ,height ? height : size.cy);
}


Listener<RadioButton::Check&>& RadioButton::onCheck() { return _onCheck; }
Listener<RadioButton::Paint&>& RadioButton::onPaint() { return _onPaint; }


bool RadioButton::pushLike() const {
	return _handle.hasStyle(BS_PUSHLIKE);
}


void RadioButton::pushLike(bool value) {
	_handle.setStyle(BS_PUSHLIKE, value);
}


bool RadioButton::rightButton() const {
	return _handle.hasStyle(BS_RIGHTBUTTON);
}


void RadioButton::rightButton(bool value) {
	_handle.setStyle(BS_RIGHTBUTTON, value);
}


bool RadioButton::firstOfGroup() const {
	return _handle.hasStyle(WS_GROUP);
}


void RadioButton::firstOfGroup(bool value) {
	_handle.setStyle(WS_GROUP, value);
}


void RadioButton::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ERASEBKGND : {
			if (!onPaint()) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_MOVE : {
			ButtonBase::processMessage(msg);
			auto parent = this->parent();
			if (parent && parent->brush() && !onPaint()) {
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
			if (HIWORD(msg.wparam) == BN_CLICKED) {
				if (checked() || !autoCheck()) { // チェックされてない状態でフォーカスをセットされた場合もここに来るようだ
					if (!_checkedChanging) {
						_checkedChanging = true;
						scopeExit([&] () {
							_checkedChanging = false;
						});
						Check event(*this);
						onCheck()(event);
					}
				}
			}
			ButtonBase::processMessage(msg);
		} break;
		case wmReflect + WM_CTLCOLORSTATIC : {
			if (!onPaint()) {
				HBRUSH brush = this->brush();
				auto origin = brushOrigin();
				if (!brush) {
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
				} break;
			}
			ButtonBase::processMessage(msg);
		} break;
		case WM_SETFOCUS : {
			ButtonBase::processMessage(msg);
			if (autoCheck() && !Mouse::isPushed(Mouse::lButton)) {
				checked(true);
			}
		} break;
		default : {
			ButtonBase::processMessage(msg);
		} break;
	}
}



	}
}