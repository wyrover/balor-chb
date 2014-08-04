#include "UpDown.hpp"

#include <utility>

#include <balor/gui/Edit.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::move;


namespace {
static_assert(UpDown::Options::noArrowKeys == UDS_ARROWKEYS, "Invalid enum value");
static_assert(UpDown::Options::horizontal  == UDS_HORZ, "Invalid enum value");
static_assert(UpDown::Options::leftAlign   == UDS_ALIGNLEFT, "Invalid enum value");

const int optionsMask = UDS_ARROWKEYS | UDS_HORZ | UDS_ALIGNLEFT;
const int optionsXorMask = UDS_ARROWKEYS;
} // namespace



bool UpDown::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



UpDown::UpDown() {
}


UpDown::UpDown(UpDown&& value, bool checkSlicing)
	: Control(move(value))
	, _buddySize(move(value._buddySize))
	, _onDown(move(value._onDown))
	, _onUp(move(value._onUp))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


UpDown::UpDown(Control& parent, int x, int y, int width, int height, UpDown::Options options) {
	assert("Invalid UpDown::Options" && Options::_validate(options));
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_BAR_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(0, UPDOWN_CLASSW, nullptr
		, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		| (options ^ optionsXorMask) | (options & Options::leftAlign ? 0 : UDS_ALIGNRIGHT)
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
}


UpDown::UpDown(Control& parent, Control& buddy, UpDown::Options options) {
	assert("buddy not created" && buddy);
	assert("buddy's parent is different from parent" && buddy.parent() == &parent);
	*this = UpDown(parent, 0, 0, 0, 0, options);
	this->buddy(&buddy);
}


UpDown::~UpDown() {
}


UpDown& UpDown::operator=(UpDown&& value) {
	if (&value != this) {
		this->~UpDown();
		new (this) UpDown(move(value));
	}
	return *this;
}


Control* UpDown::buddy() const {
	auto buddy = (HWND)SendMessageW(handle(), UDM_GETBUDDY, 0, 0);
	return fromHandle(buddy);
}


void UpDown::buddy(Control* value) {
	if (value) {
		if (value == buddy()) {
			value->size(_buddySize); // UDM_SETBUDDY を送ると UpDown の分だけ buddy が小さくなるので位置追跡の為に buddy を再設定した場合は保存していた大きさを復元する
		} else {
			_buddySize = value->size(); // 復元用に大きさを保存しておく
		}
	}
	SendMessageW(handle(), UDM_SETBUDDY, (WPARAM)(value ? static_cast<HWND>(*value) : nullptr), 0);
}


bool UpDown::focusable() const {
	return false; // 方向キーでフォーカスを得ないように上書き
}


Listener<UpDown::Down&>& UpDown::onDown() { return _onDown; }
Listener<UpDown::Up&>& UpDown::onUp() { return _onUp; }


UpDown::Options UpDown::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


void UpDown::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_NOTIFY : {
			auto info = (NMUPDOWN*)msg.lparam;
			if (info->hdr.code == UDN_DELTAPOS) {
				if (0 < info->iDelta) {
					Down event(*this);
					onDown()(event);
				} else {
					Up event(*this);
					onUp()(event);
				}
			}
			Control::processMessage(msg);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}