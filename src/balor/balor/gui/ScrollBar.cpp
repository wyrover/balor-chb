#include "ScrollBar.hpp"

#include <algorithm>
#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using namespace balor::graphics;


namespace {
static_assert(ScrollBar::Options::vertical == SBS_VERT, "Invalid enum value");

const int optionsMask = SBS_VERT;
const int optionsXorMask = 0;
} // namespace


bool ScrollBar::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



ScrollBar::Scroll::Scroll(ScrollBar& sender, int oldValue, int newValue, bool tracking)
	: ScrollBar::Event(sender), _oldValue(oldValue), _newValue(newValue), _tracking(tracking) {
}


int ScrollBar::Scroll::newValue() const { return _newValue; }
int ScrollBar::Scroll::oldValue() const { return _oldValue; }
bool ScrollBar::Scroll::tracking() const { return _tracking; }



ScrollBar::ScrollBar() {
}


ScrollBar::ScrollBar(Control& parent, int x, int y, int width, int height, int minimum, int maximum, int pageSize, ScrollBar::Options options) : _lineSize(1) {
	assert("pageSize out of range" && 0 <= pageSize);
	assert("pageSize out of range" && pageSize <= maximum - minimum + 1);
	assert("maximum is smaller than minimum" && minimum <= maximum);
	assert("Invalid ScrollBar::Options" && Options::_validate(options));

	attachHandle(CreateWindowExW(0, L"SCROLLBAR", nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | (options ^ optionsXorMask)  
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	info.nMax = maximum;
	info.nMin = minimum;
	info.nPage = pageSize;
	info.nPos = minimum;
	SetScrollInfo(handle(), SB_CTL, &info, TRUE);
	size(getPreferredSize(width, height));
}


ScrollBar::ScrollBar(ScrollBar&& value, bool checkSlicing)
	: Control(move(value))
	, _lineSize(move(value._lineSize))
	, _onScroll(move(value._onScroll))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ScrollBar::~ScrollBar() {
}


ScrollBar& ScrollBar::operator=(ScrollBar&& value) {
	if (&value != this) {
		this->~ScrollBar();
		new (this) ScrollBar(move(value));
	}
	return *this;
}


int ScrollBar::defaultHorizontalHeight() {
	return GetSystemMetrics(SM_CYHSCROLL);
}


int ScrollBar::defaultVerticalWidth() {
	return GetSystemMetrics(SM_CXVSCROLL);
}


Size ScrollBar::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	Size size;
	if (options() & Options::vertical) {
		size.width  = defaultVerticalWidth();
		size.height = (maximum() - minimum()) * verticalArrowHeight()  / 10;
	} else {
		size.width  = (maximum() - minimum()) * horizontalArrowWidth() / 10;
		size.height = defaultHorizontalHeight();
	}
	return Size(width  ? width  : size.width
			   ,height ? height : size.height);
}


int ScrollBar::horizontalArrowWidth() {
	return GetSystemMetrics(SM_CXHSCROLL);
}


int ScrollBar::lineSize() const {
	return _lineSize;
}


void ScrollBar::lineSize(int value) {
	assert("Negative lineSize" && 0 <= value);
	_lineSize = value;
}


int ScrollBar::maximum() const {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_RANGE;
	verify(GetScrollInfo(handle(), SB_CTL, &info));
	return info.nMax;
}


void ScrollBar::maximum(int value) {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_RANGE;
	verify(GetScrollInfo(handle(), SB_CTL, &info));
	if (value != info.nMax) {
		info.fMask = SIF_RANGE;
		info.nMax = value;
		if (value < info.nMin) {
			info.nMin = value;
		}
		SetScrollInfo(handle(), SB_CTL, &info, TRUE);
	}
}


int ScrollBar::minimum() const {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_RANGE;
	verify(GetScrollInfo(handle(), SB_CTL, &info));
	return info.nMin;
}


void ScrollBar::minimum(int value) {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_RANGE;
	verify(GetScrollInfo(handle(), SB_CTL, &info));
	if (value != info.nMin) {
		info.fMask = SIF_RANGE;
		if (info.nMax < value) {
			info.nMax = value; // ‚â‚ç‚È‚­‚Æ‚àŽ©“®“I‚É‚±‚¤‚È‚é‚ªˆê‰žB
		}
		info.nMin = value;
		SetScrollInfo(handle(), SB_CTL, &info, TRUE);
	}
}


Listener<ScrollBar::Scroll&>& ScrollBar::onScroll() { return _onScroll; }


ScrollBar::Options ScrollBar::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


int ScrollBar::pageSize() const {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_PAGE;
	verify(GetScrollInfo(handle(), SB_CTL, &info));
	return info.nPage;
}


void ScrollBar::pageSize(int value) {
	assert("pageSize out of range" && 0 <= value);
	assert("pageSize out of range" && value <= maximum() - minimum() + 1);
	if (value != pageSize()) {
		SCROLLINFO info;
		info.cbSize = sizeof(info);
		info.fMask = SIF_PAGE;
		info.nPage = value;
		SetScrollInfo(handle(), SB_CTL, &info, TRUE);
	}
}


int ScrollBar::value() const {
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_POS;
	verify(GetScrollInfo(handle(), SB_CTL, &info));
	return info.nPos;
}


void ScrollBar::value(int value) {
	assert("value out of range" && minimum() <= value);
	assert("value out of range" && value <= maximum() - pageSize() + 1);
	const int oldValue = this->value();
	if (value != oldValue) {
		SCROLLINFO info;
		info.fMask = SIF_POS;
		info.nPos = value;
		SetScrollInfo(handle(), SB_CTL, &info, TRUE);
		Scroll event(*this, oldValue, value, false);
		onScroll()(event);
	}
}


int ScrollBar::verticalArrowHeight() {
	return GetSystemMetrics(SM_CYVSCROLL);
}


void ScrollBar::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_HSCROLL :
		case wmReflect + WM_VSCROLL : {
			SCROLLINFO info;
			info.cbSize = sizeof(info);
			info.fMask = SIF_ALL;
			verify(GetScrollInfo(handle(), SB_CTL, &info));
			int pos = info.nPos;
			bool tracking = false;
			switch (LOWORD(msg.wparam)) {
				case SB_THUMBPOSITION : pos = info.nTrackPos; break;
				case SB_THUMBTRACK    : pos = info.nTrackPos; tracking = true; break;
				case SB_LINEUP        : pos = pos - lineSize(); break;
				case SB_LINEDOWN      : pos = pos + lineSize(); break;
				case SB_PAGEUP        : pos = pos - static_cast<int>(info.nPage); break;
				case SB_PAGEDOWN      : pos = pos + static_cast<int>(info.nPage); break;
				case SB_TOP           : pos = 0; break;
				case SB_BOTTOM        : pos = info.nMax - info.nPage + 1; break;
			}
			pos = min(pos, info.nMax - static_cast<int>(info.nPage) + 1);
			pos = max(pos, info.nMin);
			const int oldPos = info.nPos;
			if (pos != oldPos) {
				info.fMask = SIF_POS;
				info.nPos = pos;
				SetScrollInfo(handle(), SB_CTL, &info, TRUE);
			}
			if (pos != oldPos || LOWORD(msg.wparam) == SB_THUMBPOSITION) {
				Scroll event(*this, info.nPos, pos, tracking);
				onScroll()(event);
			}
		} break;
		case wmReflect + WM_CTLCOLORSCROLLBAR : {
			HBRUSH brush = this->brush();
			if (brush) {
				Graphics graphics((HDC)msg.wparam);
				graphics.brushOrigin(brushOrigin());
				msg.result = (LRESULT)brush;
			} else {
				Control::processMessage(msg);
			}
		} break;
		case WM_MOUSEWHEEL : {
			SCROLLINFO info;
			info.cbSize = sizeof(info);
			info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
			verify(GetScrollInfo(handle(), SB_CTL, &info));
			int delta = GET_WHEEL_DELTA_WPARAM(msg.wparam);
			delta = (delta / WHEEL_DELTA) * lineSize();
			int pos = info.nPos - delta;
			pos = min(pos, info.nMax - static_cast<int>(info.nPage) + 1);
			pos = max(pos, info.nMin);
			value(pos);
			Control::processMessage(msg);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}