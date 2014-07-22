#include "ProgressBar.hpp"

#include <algorithm>
#include <utility>

#include <balor/gui/ScrollBar.hpp>
#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using namespace balor::system;



ProgressBar::ProgressBar() {
}


ProgressBar::ProgressBar(ProgressBar&& value, bool checkSlicing)
	: Control(move(value)) {
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ProgressBar::ProgressBar(Control& parent, int x, int y, int width, int height, int minimum, int maximum) {
	assert("minimum out of range" && 0 <= minimum);
	assert("minimum out of range" && minimum <= maximum);
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_PROGRESS_CLASS};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(0, PROGRESS_CLASSW, nullptr
		, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	SendMessageW(handle(), PBM_SETRANGE32, minimum, maximum);
	size(getPreferredSize(width, height));
}


ProgressBar::~ProgressBar() {
}


ProgressBar& ProgressBar::operator=(ProgressBar&& value) {
	if (&value != this) {
		this->~ProgressBar();
		new (this) ProgressBar(move(value));
	}
	return *this;
}


void ProgressBar::add(int delta) {
	assert("Cant't add value" && !indeterminate());
	SendMessageW(handle(), PBM_DELTAPOS, delta, 0);
}


bool ProgressBar::focusable() const {
	return false; // 方向キーでフォーカスを得ないように上書き。
}


Size ProgressBar::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	int length = (maximum() - minimum()) * ScrollBar::defaultVerticalWidth() / 10; // かなり適当。
	int thickness = ScrollBar::defaultHorizontalHeight();
	Size size = vertical() ? Size(thickness, length) : Size(length, thickness);
	return Size(width  ? width  : size.width
			   ,height ? height : size.height);
}


bool ProgressBar::indeterminate() const {
	return _handle.hasStyle(PBS_MARQUEE);
}


void ProgressBar::indeterminate(bool value, int animationSpeed) {
	assert("This function needs visual style" && System::visualStyleEnabled());
	if (value != indeterminate()) {
		_handle.style(toFlag(_handle.style()).set(PBS_MARQUEE, value));
		_handle.updateStyle();
		if (value) {
			SendMessageW(handle(), PBM_SETMARQUEE, TRUE, animationSpeed);
		}
	}
}


int ProgressBar::maximum() const {
	return SendMessageW(handle(), PBM_GETRANGE, FALSE, 0);
}


void ProgressBar::maximum(int value) {
	assert("Negative maximum" && 0 <= value);
	PBRANGE range;
	SendMessageW(handle(), PBM_GETRANGE, 0, (LPARAM)&range);
	if (value != range.iHigh) {
		SendMessageW(handle(), PBM_SETRANGE32, min(value, range.iLow), value);
	}
}


int ProgressBar::minimum() const {
	return SendMessageW(handle(), PBM_GETRANGE, TRUE, 0);
}


void ProgressBar::minimum(int value) {
	assert("Negative minimum" && 0 <= value);
	PBRANGE range;
	SendMessageW(handle(), PBM_GETRANGE, 0, (LPARAM)&range);
	if (value != range.iLow) {
		SendMessageW(handle(), PBM_SETRANGE32, value, max(value, range.iHigh));
	}
}


int ProgressBar::value() const {
	return SendMessageW(handle(), PBM_GETPOS, 0, 0);
}


void ProgressBar::value(int value) {
	assert("Cant't set value" && !indeterminate());
	assert("value out of range" && minimum() <= value);
	assert("value out of range" && value <= maximum());
	if (value != this->value()) {
		SendMessageW(handle(), PBM_SETPOS, value, 0);
	}
}


bool ProgressBar::vertical() const {
	return _handle.hasStyle(PBS_VERTICAL);
}


void ProgressBar::vertical(bool value) {
	_handle.setStyle(PBS_VERTICAL, value);
}



	}
}