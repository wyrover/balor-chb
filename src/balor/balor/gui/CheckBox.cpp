#include "CheckBox.hpp"

#include <utility>

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



bool CheckBox::State::_validate(State value) {
	return unchecked <= value && value <= indeterminate;
}



CheckBox::CheckBox() : _stateChanging(false) {
}


CheckBox::CheckBox(CheckBox&& value, bool checkSlicing)
	: ButtonBase(move(value))
	, _stateChanging(move(value._stateChanging))
	, _onPaint(move(value._onPaint))
	, _onStateChange(move(value._onStateChange))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


CheckBox::CheckBox(Control& parent, int x, int y, int width, int height, StringRange text, Listener<CheckBox::StateChange&> onStateChange)
	: _stateChanging(false)
	, _onStateChange(move(onStateChange)) {
	attachHandle(CreateWindowExW(0, L"BUTTON", text.c_str()
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | BS_AUTOCHECKBOX | TextAlign::middleLeft
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	size(getPreferredSize(width, height));
}


CheckBox::~CheckBox() {
}


CheckBox& CheckBox::operator=(CheckBox&& value) {
	if (&value != this) {
		this->~CheckBox();
		new (this) CheckBox(move(value));
	}
	return *this;
}


bool CheckBox::autoCheck() const {
	auto style = toFlag(_handle.style());
	return style[BS_AUTOCHECKBOX] || style[BS_AUTO3STATE]; // ビットが重なり合っていることに注意
}


void CheckBox::autoCheck(bool value) {
	if (value != autoCheck()) {
		auto style = toFlag(_handle.style()).set(BS_CHECKBOX | BS_AUTOCHECKBOX | BS_3STATE | BS_AUTO3STATE, false);
		if (threeState()) { // ビットが重なり合っていることに注意
			style.set(value ? BS_AUTO3STATE : BS_3STATE, true);
		} else {
			style.set(value ? BS_AUTOCHECKBOX : BS_CHECKBOX, true);
		}
		_handle.style(style);
		_handle.updateStyle();
	}
}


bool CheckBox::checked() const {
	return state() != State::unchecked;
}


void CheckBox::checked(bool value) {
	state(value ? State::checked : State::unchecked);
}


Size CheckBox::getPreferredSize(int width, int height) const {
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


Listener<CheckBox::Paint&>& CheckBox::onPaint() { return _onPaint; }
Listener<CheckBox::StateChange&>& CheckBox::onStateChange() { return _onStateChange; }


bool CheckBox::pushLike() const {
	return _handle.hasStyle(BS_PUSHLIKE);
}


void CheckBox::pushLike(bool value) {
	_handle.setStyle(BS_PUSHLIKE, value);
}


bool CheckBox::rightButton() const {
	return _handle.hasStyle(BS_RIGHTBUTTON);
}


void CheckBox::rightButton(bool value) {
	_handle.setStyle(BS_RIGHTBUTTON, value);
}


CheckBox::State CheckBox::state() const {
	return static_cast<State>(SendMessageW(handle(), BM_GETCHECK, 0, 0));
}


void CheckBox::state(CheckBox::State value) {
	assert("Invalid CheckBox::State" && State::_validate(value));
	if (value != state()) {
		SendMessageW(handle(), BM_SETCHECK, value, 0);
		if (!_stateChanging) {
			_stateChanging = true;
			scopeExit([&] () {
				_stateChanging = false;
			});
			Event event(*this);
			onStateChange()(event);
		}
	}
}


bool CheckBox::threeState() const {
	auto style = toFlag(_handle.style());
	return style[BS_3STATE] || style[BS_AUTO3STATE]; // ビットが重なり合っていることに注意
}


void CheckBox::threeState(bool value) {
	if (value != threeState()) {
		auto style = toFlag(_handle.style()).set(BS_CHECKBOX | BS_AUTOCHECKBOX | BS_3STATE | BS_AUTO3STATE, false);
		if (autoCheck()) { // ビットが重なり合っていることに注意
			style.set(value ? BS_AUTO3STATE : BS_AUTOCHECKBOX, true);
		} else {
			style.set(value ? BS_3STATE : BS_CHECKBOX, true);
		}
		_handle.style(style);
		_handle.updateStyle();
	}
}


void CheckBox::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ERASEBKGND : {
			if (!onPaint()) {
				processMessageByDefault(msg);
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
			if (HIWORD(msg.wparam) == BN_CLICKED) {
				if (!_stateChanging) { // 無限再帰防止
					_stateChanging = true;
					scopeExit([&] () {
						_stateChanging = false;
					});
					StateChange event(*this);
					onStateChange()(event);
				}
			}
			ButtonBase::processMessage(msg);
		} break;
		case wmReflect + WM_CTLCOLORSTATIC : {
			if (!onPaint()) {
				HBRUSH brush = this->brush();
				auto origin = brushOrigin();
				if (!brush) { // 親コントロールと地続きな背景を持つので親のブラシがあれば使う ;
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