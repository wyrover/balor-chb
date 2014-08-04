#include "ToolTip.hpp"

#include <utility>

#include <balor/graphics/Color.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <WindowsX.h>


namespace balor {
	namespace gui {

using std::move;
using namespace balor::graphics;
using namespace balor::system;


namespace {
static_assert(ToolTip::Options::center      == TTF_CENTERTIP, "Invalid enum value");
static_assert(ToolTip::Options::track       == TTF_TRACK, "Invalid enum value");
static_assert(ToolTip::Options::absolute    == TTF_ABSOLUTE, "Invalid enum value");
static_assert(ToolTip::Options::transparent == TTF_TRANSPARENT, "Invalid enum value");
static_assert(ToolTip::Options::parseLinks  == TTF_PARSELINKS, "Invalid enum value");


const int maxToolTipLength = 2048;


struct ToolTipInfo : public TOOLINFOW {
	ToolTipInfo(Control& owner, int id = -1) {
		ZeroMemory(this, sizeof(TOOLINFOW));
		cbSize = sizeof(TOOLINFOW);
		hwnd = owner;
		uId = id == -1 ? reinterpret_cast<UINT_PTR>(owner.handle()) : id;
	}
};
} // namespace



bool ToolTip::Options::_validate(Options value) {
	return (value & ~(none | center | track | absolute | transparent | parseLinks)) == 0;
}



ToolTip::Event::Event(ToolTip& sender, Control& owner, int id)
	: EventWithSender<ToolTip>(sender), _owner(owner), _id(id) {
}


int ToolTip::Event::id() const { return _id; }
Control& ToolTip::Event::owner() { return _owner; }



ToolTip::GetText::GetText(ToolTip& sender, Control& owner, int id)
	: ToolTip::Event(sender, owner, id) {
}


String& ToolTip::GetText::text() { return _text; }
void ToolTip::GetText::text(StringRange value) { _text = String(value.c_str(), value.length()); }



ToolTip::Popup::Popup(ToolTip& sender, Control& owner, int id)
	: ToolTip::Event(sender, owner, id), _positionChanged(false) {
}


Point ToolTip::Popup::position() const { return sender().position(); }
void ToolTip::Popup::position(const Point& value) { sender().position(value); _positionChanged = true; }
void ToolTip::Popup::position(int x, int y) { position(Point(x, y)); }



ToolTip::ToolTip()
	: _owned(true)
	, _textColor(Color::controlText())
	, _textFormat(Graphics::TextFormat::none)
	{
}


ToolTip::ToolTip(ToolTip&& value, bool checkSlicing)
	: Control(move(value))
	, _owned(move(value._owned))
	, _temporaryText(move(value._temporaryText))
	, _textColor(move(value._textColor))
	, _textFormat(move(value._textFormat))
	, _onGetText(move(value._onGetText))
	, _onLinkClick(move(value._onLinkClick))
	, _onPop(move(value._onPop))
	, _onPopup(move(value._onPopup))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ToolTip::ToolTip(Control& parent)
	: _owned(true)
	, _textColor(Color::controlText())
	, _textFormat(Graphics::TextFormat::none)
	{
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_TAB_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(0, TOOLTIPS_CLASSW, nullptr
		, TTS_ALWAYSTIP
		, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
		, parent, nullptr,nullptr, nullptr));
}


ToolTip::ToolTip(HWND handle)
	: _owned(false)
	, _textColor(Color::controlText())
	, _textFormat(Graphics::TextFormat::none)
	{
	attachHandle(handle);
}


ToolTip::~ToolTip() {
	if (IsWindow(_handle)) {
		if (_owned) {
			destroyHandle();
		} else {
			_handle.procedure(_defaultProcedure);
			_handle.control(nullptr);
			_handle = Handle();
		}
	}
}


ToolTip& ToolTip::operator=(ToolTip&& value) {
	if (&value != this) {
		this->~ToolTip();
		new (this) ToolTip(move(value));
	}
	return *this;
}


void ToolTip::activate(bool value) {
	SendMessageW(handle(), TTM_ACTIVATE, value ? TRUE : FALSE, 0);
}


bool ToolTip::alwaysPopup() const {
	return _handle.hasStyle(TTS_ALWAYSTIP);
}


void ToolTip::alwaysPopup(bool value) {
	_handle.setStyle(TTS_ALWAYSTIP, value);
}


bool ToolTip::animate() const {
	return !_handle.hasStyle(TTS_NOANIMATE);
}


void ToolTip::animate(bool value) {
	_handle.setStyle(TTS_NOANIMATE, !value);
}


bool ToolTip::balloon() const {
	return _handle.hasStyle(TTS_BALLOON);
}


void ToolTip::balloon(bool value) {
	_handle.setStyle(TTS_BALLOON, value);
}


Rectangle ToolTip::boundsFromTextBounds(const Rectangle& textBounds) const {
	RECT rect = textBounds;
	verify(SendMessageW(handle(), TTM_ADJUSTRECT, TRUE, (LPARAM)&rect));
	return rect;
}


Rectangle ToolTip::boundsFromTextBounds(int x, int y, int width, int height) const {
	return boundsFromTextBounds(Rectangle(x, y, width, height));
}


void ToolTip::clear() {
	for (;;) {
		TOOLINFOW info;
		ZeroMemory(&info, sizeof(info));
		info.cbSize = sizeof(info);
		if (!SendMessageW(handle(), TTM_ENUMTOOLSW, 0, (LPARAM)&info)) {
			break;
		}
		SendMessageW(handle(), TTM_DELTOOLW, 0, (LPARAM)&info);
	}
}


bool ToolTip::closeButton() const {
	return !_handle.hasStyle(TTS_CLOSE);
}


void ToolTip::closeButton(bool value) {
	_handle.setStyle(TTS_CLOSE, !value);
}


bool ToolTip::fade() const {
	return !_handle.hasStyle(TTS_NOFADE);
}


void ToolTip::fade(bool value) {
	_handle.setStyle(TTS_NOFADE, !value);
}


String ToolTip::get(Control& control, int id) const {
	assert("id out of range" && -1 <= id);
	ToolTipInfo info(control, id);
	wchar_t buffer[maxToolTipLength];
	buffer[0] = 0;
	info.lpszText = buffer;
	SendMessageW(handle(), TTM_GETTEXTW, 0, (LPARAM)&info);
	return info.lpszText;
}


int ToolTip::initialDelay() const {
	return SendMessageW(handle(), TTM_GETDELAYTIME, TTDT_INITIAL, 0);
}


void ToolTip::initialDelay(int value) {
	assert("initialDelay out of range" && 0 <= value);
	assert("initialDelay out of range" && value <= SHRT_MAX);
	SendMessageW(handle(), TTM_SETDELAYTIME, TTDT_INITIAL, static_cast<WORD>(value));
}


int ToolTip::maxTextWidth() const {
	return SendMessageW(handle(), TTM_GETMAXTIPWIDTH, 0, 0);
}


void ToolTip::maxTextWidth(int value) {
	assert("maxTextWidth out of range" && -1 <= value);
	SendMessageW(handle(), TTM_SETMAXTIPWIDTH, 0, value);
}


bool ToolTip::noPrefix() const {
	return _handle.hasStyle(TTS_NOPREFIX);
}


void ToolTip::noPrefix(bool value) {
	_handle.setStyle(TTS_NOPREFIX, value);
}


Listener<ToolTip::GetText&>& ToolTip::onGetText() { return _onGetText; }
Listener<ToolTip::LinkClick&>& ToolTip::onLinkClick() { return _onLinkClick; }
Listener<ToolTip::Pop&>& ToolTip::onPop() { return _onPop; }
Listener<ToolTip::Popup&>& ToolTip::onPopup() { return _onPopup; }


void ToolTip::pop() {
	TOOLINFOW info;
	ZeroMemory(&info, sizeof(info));
	info.cbSize = sizeof(info);
	if (SendMessageW(handle(), TTM_GETCURRENTTOOLW, 0, (LPARAM)&info) != 0) {
		if (info.uFlags & TTF_TRACK) { // track の場合を分けないと二度と表示されなくなる
			SendMessageW(handle(), TTM_TRACKACTIVATE, FALSE, (LPARAM)&info);
		} else {
			SendMessageW(handle(), TTM_POP, 0, 0);
		}
	}
}


void ToolTip::popup() {
	assert("This function needs visual style" && System::visualStyleEnabled());
	SendMessageW(handle(), TTM_POPUP, 0, 0);
}


int ToolTip::popDelay() const {
	return SendMessageW(handle(), TTM_GETDELAYTIME, TTDT_AUTOPOP, 0);
}


void ToolTip::popDelay(int value) {
	assert("popDelay out of range" && 0 <= value);
	assert("popDelay out of range" && value <= SHRT_MAX);
	SendMessageW(handle(), TTM_SETDELAYTIME, TTDT_AUTOPOP, static_cast<WORD>(value));
}


void ToolTip::reset(Control& control, int id) {
	ToolTipInfo info(control, id);
	SendMessageW(handle(), TTM_DELTOOLW, 0, (LPARAM)&info);
}


void ToolTip::resetDelay() {
	setDelay(-1);
}


int ToolTip::reshowDelay() const {
	return SendMessageW(handle(), TTM_GETDELAYTIME, TTDT_RESHOW, 0);
}


void ToolTip::reshowDelay(int value) {
	assert("reshowDelay out of range" && 0 <= value);
	assert("reshowDelay out of range" && value <= SHRT_MAX);
	SendMessageW(handle(), TTM_SETDELAYTIME, TTDT_RESHOW, static_cast<WORD>(value));
}


void ToolTip::set(Control& control, ToolTip::Options options) {
	set(control, -1, Rectangle(0, 0, 0, 0), options);
}


void ToolTip::set(Control& control, StringRange toolTip, ToolTip::Options options) {
	set(control, -1, Rectangle(0, 0, 0, 0), toolTip, options);
}


void ToolTip::set(Control& control, int id, const Rectangle& bounds, ToolTip::Options options) {
	set(control, id, bounds, LPSTR_TEXTCALLBACKW, options);
}


void ToolTip::set(Control& control, int id, int x, int y, int width, int height, ToolTip::Options options) {
	set(control, id, Rectangle(x, y, width, height), options);
}


void ToolTip::set(Control& control, int id, const Rectangle& bounds, StringRange toolTip, ToolTip::Options options) {
	assert("id out of range" && -1 <= id);
	assert("Invalid ToolTip::Options" && Options::_validate(options));
	assert("Too long toolTip text" && (toolTip.c_str() == LPSTR_TEXTCALLBACKW || toolTip.length() < maxToolTipLength));
	ToolTipInfo info(control, id);
	// いくつかの事例（Options::parseLinks のオンオフ、文字列設定の有無）においていったん消さなければ更新されない場合があるので強制的に消す
	SendMessageW(handle(), TTM_DELTOOLW, 0, (LPARAM)&info);
	info.uFlags = TTF_SUBCLASS | options | (id == -1 ? TTF_IDISHWND : 0);
	info.rect = bounds;
	info.lpszText = const_cast<LPWSTR>(toolTip.c_str());
	verify(SendMessageW(handle(), TTM_ADDTOOLW, 0, (LPARAM)&info));
}


void ToolTip::set(Control& control, int id, int x, int y, int width, int height, StringRange toolTip, ToolTip::Options options) {
	set(control, id, Rectangle(x, y, width, height), toolTip, options);
}


void ToolTip::setDelay(int value) {
	assert("initialDelay out of range" && -1 <= value);
	assert("initialDelay out of range" && value <= SHRT_MAX);
	SendMessageW(handle(), TTM_SETDELAYTIME, TTDT_AUTOMATIC, static_cast<WORD>(value));
}


Color ToolTip::textColor() const {
	return _textColor;
}


void ToolTip::textColor(const Color& value) {
	_textColor = value;
}


Graphics::TextFormat ToolTip::textFormat() const {
	return _textFormat;
}


void ToolTip::textFormat(Graphics::TextFormat value) {
	assert("Invalid Graphics::TextFormat" && Graphics::TextFormat::_validate(value));
	_textFormat = value;
}


String ToolTip::title() const {
	assert("This function needs visual style" && System::visualStyleEnabled());
	TTGETTITLE title;
	title.dwSize = sizeof(title);
	wchar_t buffer[maxToolTipLength];
	buffer[0] = 0;
	title.pszTitle = buffer;
	title.cch = maxToolTipLength;
	SendMessageW(handle(), TTM_GETTITLE, 0, (LPARAM)&title);
	return title.pszTitle;
}


void ToolTip::title(StringRange title, HICON icon) {
	assert("Too long title text" && title.length() < maxToolTipLength);
	verify(SendMessageW(handle(), TTM_SETTITLEW, (WPARAM)icon, (LPARAM)title.c_str()));
}


void ToolTip::track(Control& control, const Point& position) {
	track(control, -1, position);
}


void ToolTip::track(Control& control, int x, int y) {
	track(control, -1, x, y);
}


void ToolTip::track(Control& control, int id, const Point& position) {
	ToolTipInfo info(control, id);
	trackPosition(position);
	SendMessageW(handle(), TTM_TRACKACTIVATE, TRUE, (LPARAM)&info);
}


void ToolTip::track(Control& control, int id, int x, int y) {
	track(control, id, Point(x, y));
}


void ToolTip::trackPosition(const Point& value) {
	SendMessageW(handle(), TTM_TRACKPOSITION, 0, MAKELPARAM(value.x, value.y));
}


void ToolTip::trackPosition(int x, int y) {
	trackPosition(Point(x, y));
}


void ToolTip::update() {
	SendMessageW(handle(), TTM_UPDATE, 0, 0);
}



bool ToolTip::visible() const {
	return SendMessageW(handle(), TTM_GETCURRENTTOOLW, 0, 0) != 0;
}


void ToolTip::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				case NM_CUSTOMDRAW : {
					auto info = reinterpret_cast<NMTTCUSTOMDRAW*>(msg.lparam);
					if (info->nmcd.dwDrawStage == CDDS_PREPAINT) {
						if (textColor() != Color::controlText()) {
							Graphics graphics(info->nmcd.hdc);
							graphics.textColor(textColor());
						}
						info->uDrawFlags |= textFormat();
						return;
					}
				} break;
				case TTN_GETDISPINFOW : {
					if (onGetText()) {
						auto info = (NMTTDISPINFOW*)msg.lparam;
						auto owner = fromHandle(msg.handle);
						GetText event(*this, *owner, info->hdr.idFrom);
						onGetText()(event);
						_temporaryText = move(event.text());
						info->lpszText = const_cast<wchar_t*>(_temporaryText.c_str());
						return;
					}
				} break;
				case TTN_LINKCLICK : {
					if (onLinkClick()) {
						auto info = (NMHDR*)msg.lparam;
						auto owner = fromHandle(msg.handle);
						LinkClick event(*this, *owner, info->idFrom);
						onLinkClick()(event);
						return;
					}
				} break;
				case TTN_POP : {
					if (onPop()) {
						auto info = (NMHDR*)msg.lparam;
						auto owner = fromHandle(msg.handle);
						Pop event(*this, *owner, info->idFrom);
						onPop()(event);
						return;
					}
				} break;
				case TTN_SHOW : {
					if (onPopup()) {
						auto info = (NMHDR*)msg.lparam;
						auto owner = fromHandle(msg.handle);
						Popup event(*this, *owner, info->idFrom);
						onPopup()(event);
						msg.result = event._positionChanged ? TRUE : 0;
						return;
					}
				} break;
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