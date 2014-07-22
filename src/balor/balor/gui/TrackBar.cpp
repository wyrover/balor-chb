#include "TrackBar.hpp"

#include <algorithm>
#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Scaler.hpp>
#include <balor/gui/ScrollBar.hpp>
#include <balor/gui/ToolTip.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <WindowsX.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;


namespace {
static_assert(TrackBar::Options::valueTip == TBS_TOOLTIPS, "Invalid enum value");

const int optionsMask = TBS_TOOLTIPS;
const int optionsXorMask = 0;

static_assert(TrackBar::TickStyle::left   == TBS_LEFT  , "Invalid enum value");
static_assert(TrackBar::TickStyle::top    == TBS_TOP   , "Invalid enum value");
static_assert(TrackBar::TickStyle::right  == TBS_RIGHT , "Invalid enum value");
static_assert(TrackBar::TickStyle::bottom == TBS_BOTTOM, "Invalid enum value");
static_assert(TrackBar::TickStyle::both   == TBS_BOTH  , "Invalid enum value");

static_assert(TrackBar::ValueTipSide::top    == TBTS_TOP   , "Invalid enum value");
static_assert(TrackBar::ValueTipSide::left   == TBTS_LEFT  , "Invalid enum value");
static_assert(TrackBar::ValueTipSide::bottom == TBTS_BOTTOM, "Invalid enum value");
static_assert(TrackBar::ValueTipSide::right  == TBTS_RIGHT , "Invalid enum value");
} // namespace



bool TrackBar::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}


bool TrackBar::TickStyle::_validate(TickStyle value) {
	switch (value) {
		case left :
		//case top :
		case right :
		//case bottom :
		case both : return true;
		default : return false;
	}
}


bool TrackBar::ValueTipSide::_validate(ValueTipSide value) {
	return top <= value && value <= right;
}



TrackBar::ValueChange::ValueChange(TrackBar& sender, int oldValue, int newValue, bool tracking)
	: TrackBar::Event(sender), _oldValue(oldValue), _newValue(newValue), _tracking(tracking) {
}


int TrackBar::ValueChange::newValue() const { return _newValue; }
int TrackBar::ValueChange::oldValue() const { return _oldValue; }
bool TrackBar::ValueChange::tracking() const { return _tracking; }



TrackBar::TrackBar() {
}


TrackBar::TrackBar(TrackBar&& value, bool checkSlicing)
	: Control(move(value))
	, _value(value._value)
	, _onValueChange(move(value._onValueChange))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


TrackBar::TrackBar(Control& parent, int x, int y, int width, int height, int minimum, int maximum, TrackBar::Options options) {
	assert("maximum is smaller than minimum" && minimum <= maximum);
	assert("Invalid TrackBar::Options" && Options::_validate(options));

	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_BAR_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(0, TRACKBAR_CLASSW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		| TBS_NOTICKS | TBS_BOTH | TBS_AUTOTICKS | (options ^ optionsXorMask)
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	this->minimum(minimum);
	this->maximum(maximum);
	_value = value();
	size(getPreferredSize(width, height));
}


TrackBar::~TrackBar() {
}


TrackBar& TrackBar::operator=(TrackBar&& value) {
	if (&value != this) {
		this->~TrackBar();
		new (this) TrackBar(move(value));
	}
	return *this;
}


void TrackBar::clearSelection() {
	SendMessageW(handle(), TBM_CLEARSEL, TRUE, 0);
}


void TrackBar::clearTick() {
	SendMessageW(handle(), TBM_CLEARTICS, TRUE, 0);
}


bool TrackBar::downIsLeft() const {
	return _handle.hasStyle(TBS_DOWNISLEFT);
}


void TrackBar::downIsLeft(bool value) {
	_handle.setStyle(TBS_DOWNISLEFT, value);
}


void TrackBar::endSelect() {
	SendMessageW(handle(), TBM_SETSELEND, TRUE, value());
}


Size TrackBar::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	int thumbSize;
	int thickness;
	if (_handle.hasStyle(TBS_FIXEDLENGTH)) {
		thumbSize = this->thumbSize();
		int append = 3;
		if (tickVisible()) {
			append += 17;
		}
		thickness = thumbSize + Scaler().scale(append);
	} else {
		thumbSize = 25;
		thickness = thumbSize;
		if (tickVisible()) {
			switch (tickStyle()) {
				case TickStyle::top    : thickness = 33; break;
				case TickStyle::bottom : thickness = 33; break;
				case TickStyle::both   : thickness = 42; break;
			}
		}
		if (thick()) {
			thickness += 2;
		}
		thickness = Scaler().scale(thickness);
	}
	Size size;
	if (vertical()) {
		size.width  = thickness;
		size.height = (maximum() - minimum()) * thumbSize / 10;
	} else {
		size.width  = (maximum() - minimum()) * thumbSize / 10;
		size.height = thickness;
	}
	return Size(width  ? width  : size.width
			   ,height ? height : size.height);
}


int TrackBar::lineSize() const {
	return SendMessageW(handle(), TBM_GETLINESIZE, 0, 0);
}


void TrackBar::lineSize(int value) {
	assert("Negative lineSize" && 0 <= value);
	if (value != lineSize()) {
		SendMessageW(handle(), TBM_SETLINESIZE, 0, value);
	}
}


int TrackBar::maximum() const {
	return SendMessageW(handle(), TBM_GETRANGEMAX, 0, 0);
}


void TrackBar::maximum(int value) {
	if (value != maximum()) {
		int minimum = this->minimum();
		if (value < minimum) {
			this->minimum(value);
		}
		SendMessageW(handle(), TBM_SETRANGEMAX, TRUE, value);
		_value = this->value();
	}
}


int TrackBar::minimum() const {
	return SendMessageW(handle(), TBM_GETRANGEMIN, 0, 0);
}


void TrackBar::minimum(int value) {
	if (value != minimum()) {
		int maximum = this->maximum();
		if (maximum < value) {
			this->maximum(value);
		}
		SendMessageW(handle(), TBM_SETRANGEMIN, TRUE, value);
		_value = this->value();
	}
}


Listener<TrackBar::ValueChange&>& TrackBar::onValueChange() {
	return _onValueChange;
}


TrackBar::Options TrackBar::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


int TrackBar::pageSize() const {
	return SendMessageW(handle(), TBM_GETPAGESIZE, 0, 0);
}


void TrackBar::pageSize(int value) {
	assert("Negative pageSize" && 0 <= value);
	if (value != pageSize()) {
		SendMessageW(handle(), TBM_SETPAGESIZE, 0, value);
	}
}


void TrackBar::scale(const Scaler& scaler) {
	Control::scale(scaler);
	if (scalable() && _handle.hasStyle(TBS_FIXEDLENGTH)) {
		thumbSize(scaler.scale(thumbSize()));
	}
}


int TrackBar::selectionBegin() const {
	return SendMessageW(handle(), TBM_GETSELSTART, 0, 0);
}


void TrackBar::selectionBegin(int value) {
	assert("value out of range" && minimum() <= value);
	assert("value out of range" && value <= maximum());
	SendMessageW(handle(), TBM_SETSELSTART, TRUE, value);
}


int TrackBar::selectionEnd() const {
	return SendMessageW(handle(), TBM_GETSELEND, 0, 0);
}


void TrackBar::selectionEnd(int value) {
	assert("value out of range" && minimum() <= value);
	assert("value out of range" && value <= maximum());
	SendMessageW(handle(), TBM_SETSELEND, TRUE, value);
}


void TrackBar::setTick(int value) {
	assert("Tick value out of range" && minimum() <= value);
	assert("Tick value out of range" && value <= maximum());
	verify(SendMessageW(handle(), TBM_SETTIC, 0, value));
}


bool TrackBar::thick() const {
	return _handle.hasStyle(TBS_ENABLESELRANGE);
}


void TrackBar::thick(bool value) {
	_handle.setStyle(TBS_ENABLESELRANGE, value);
}


int TrackBar::thumbSize() const {
	assert("thumbSize can use only if tickStyle is TickStyle::both" && tickStyle() == TickStyle::both);
	return SendMessageW(handle(), TBM_GETTHUMBLENGTH, 0, 0);
}


void TrackBar::thumbSize(int value) {
	assert("Negative thumbSize" && 0 <= value);
	assert("thumbSize can use only if tickStyle is TickStyle::both" && tickStyle() == TickStyle::both);
	if (value != thumbSize()) {
		if (!_handle.hasStyle(TBS_FIXEDLENGTH)) {
			_handle.setStyle(TBS_FIXEDLENGTH, true);
		}
		SendMessageW(handle(), TBM_SETTHUMBLENGTH, value, 0);
	}
}


bool TrackBar::thumbVisible() const {
	return _handle.hasStyle(TBS_NOTHUMB);
}


void TrackBar::thumbVisible(bool value) {
	_handle.setStyle(TBS_NOTHUMB, value);
}


int TrackBar::tickCount() const {
	return SendMessageW(handle(), TBM_GETNUMTICS, 0, 0);
}


void TrackBar::tickFrequency(int value) {
	SendMessageW(handle(), TBM_SETTICFREQ, value, 0);
}


TrackBar::TickStyle TrackBar::tickStyle() const {
	auto style = toFlag(_handle.style());
	if (style[TBS_TOP]) {
		return TickStyle::left;
	}
	if (style[TBS_BOTH]) {
		return TickStyle::both;
	}
	return TickStyle::right;
}


void TrackBar::tickStyle(TrackBar::TickStyle value) {
	assert("Invalid TrackBar::TickStyle" && TickStyle::_validate(value));
	assert("Can't change style because thumbSize setted" && (value == TickStyle::both || !_handle.hasStyle(TBS_FIXEDLENGTH)));
	if (value != tickStyle()) {
		auto style = toFlag(_handle.style()).set(TBS_TOP | TBS_BOTH, false);
		switch (value) {
			case TickStyle::left : style.set(TBS_TOP, true); break;
			case TickStyle::both : style.set(TBS_BOTH, true); break;
		}
		_handle.style(style);
		_handle.updateStyle();
	}
}


bool TrackBar::tickVisible() const {
	return !_handle.hasStyle(TBS_NOTICKS);
}


void TrackBar::tickVisible(bool value) {
	_handle.setStyle(TBS_NOTICKS, !value);
}


int TrackBar::value() const {
	return SendMessageW(handle(), TBM_GETPOS, 0, 0);
}


void TrackBar::value(int value) {
	assert("value out of range" && minimum() <= value);
	assert("value out of range" && value <= maximum());
	if (value != _value) {
		SendMessageW(handle(), TBM_SETPOS, TRUE, value);
		value = this->value();
		if (value != _value) {
			ValueChange event(*this, _value, value, false);
			_value = value;
			onValueChange()(event);
		}
	}
}


ToolTip TrackBar::valueTip() {
	return ToolTip((HWND)SendMessageW(handle(), TBM_GETTOOLTIPS, 0, 0));
}


void TrackBar::valueTipSide(TrackBar::ValueTipSide value) {
	assert("Invalid TrackBar::ValueTipSide" && ValueTipSide::_validate(value));
	SendMessageW(handle(), TBM_SETTIPSIDE, value, 0);
}


bool TrackBar::vertical() const {
	return _handle.hasStyle(TBS_VERT);
}


void TrackBar::vertical(bool value) {
	_handle.setStyle(TBS_VERT, value);
}


void TrackBar::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_MOVE : {
			Control::processMessage(msg);
			auto parent = this->parent();
			if (parent && parent->brush()) {
				thumbVisible(!thumbVisible()); // invalidate ÇµÇΩÇæÇØÇ≈ÇÕï`âÊÇµÇƒÇ≠ÇÍÇ»Ç¢
				thumbVisible(!thumbVisible()); // ÇøÇÂÇ¡Ç∆É`ÉâÇ¬Ç≠Ç™ÅEÅE
			}
		} break;
		case wmReflect + WM_CTLCOLORSTATIC : {
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
			} else {
				Control::processMessage(msg);
			}
		} break;
		case wmReflect + WM_HSCROLL :
		case wmReflect + WM_VSCROLL : {
			int newValue = value();
			int event = LOWORD(msg.wparam);
			if (newValue != _value || event == SB_THUMBPOSITION) {
				ValueChange event(*this, _value, newValue, event == SB_THUMBTRACK);
				_value = newValue;
				onValueChange()(event);
			}
		} break;
		case WM_MOUSEWHEEL : { // lineSize ÇîΩâfÇ≥ÇπÇÈà◊Ç…èàóùÇè„èëÇ´
			int delta = GET_WHEEL_DELTA_WPARAM(msg.wparam);
			delta = -(delta / WHEEL_DELTA) * lineSize();
			int newValue = value() + delta;
			newValue = max(newValue, minimum());
			newValue = min(newValue, maximum());
			value(newValue);
			resetMouseTracking(mouseHoverTime());
			MouseWheel event(*this, msg);
			onMouseWheel()(event);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}


	}
}