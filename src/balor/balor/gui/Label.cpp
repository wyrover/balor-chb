#include "Label.hpp"

#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/gui/Scaler.hpp>
#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>
#include <balor/Flag.hpp>

#include <WindowsX.h>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::move;
using namespace balor::graphics;

static_assert(Label::TextAlign::left   == SS_LEFT  , "Invalid enum value");
static_assert(Label::TextAlign::center == SS_CENTER, "Invalid enum value");
static_assert(Label::TextAlign::right  == SS_RIGHT , "Invalid enum value");



bool Label::TextAlign::_validate(TextAlign value) {
	return left <= value && value <= right;
}



Label::Label() : _textColor(Color::controlText()) {
}


Label::Label(Label&& value, bool checkSlicing)
	: Control(move(value))
	, _textColor(move(value._textColor))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Label::Label(Control& parent, int x, int y, int width, int height, StringRange text)
	 : _textColor(Color::controlText()) {
	attachHandle(CreateWindowExW(0, L"STATIC", text.c_str()
		, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | SS_EDITCONTROL
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	size(getPreferredSize(width, height));
}


Label::Label(Control& parent, int x, int y, int width, int height, HBITMAP bitmap)
	 : _textColor(Color::controlText()) {
	attachHandle(CreateWindowExW(0, L"STATIC", nullptr
		, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | SS_EDITCONTROL | SS_BITMAP | SS_REALSIZECONTROL
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	this->bitmap(bitmap);
	size(getPreferredSize(width, height));
}


Label::Label(Control& parent, int x, int y, int width, int height, HICON icon)
	 : _textColor(Color::controlText()) {
	attachHandle(CreateWindowExW(0, L"STATIC", nullptr
		, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | SS_EDITCONTROL | SS_ICON | SS_REALSIZECONTROL
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	this->icon(icon);
	size(getPreferredSize(width, height));
}


Label::~Label() { 
}


Label& Label::operator=(Label&& value) {
	if (&value != this) {
		this->~Label();
		new (this) Label(move(value));
	}
	return *this;
}


Bitmap Label::bitmap() const {
	auto bitmap = (HBITMAP)SendMessageW(handle(), STM_GETIMAGE, IMAGE_BITMAP, 0);
	return Bitmap(bitmap);
}


void Label::bitmap(HBITMAP value) {
	assert("Non bitmap label" && _handle.hasStyle(SS_BITMAP));
	SendMessageW(handle(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)value);
}


bool Label::centerImage() const {
	return _handle.hasStyle(SS_CENTERIMAGE);
}


void Label::centerImage(bool value) {
	_handle.setStyle(SS_CENTERIMAGE, value);
}


Control::Edge Label::edge() const {
	return _handle.edge();
}


void Label::edge(Control::Edge value) {
	_handle.edge(value);
}


bool Label::endEllipsis() const {
	return _handle.hasStyle(SS_ENDELLIPSIS);
}


void Label::endEllipsis(bool value) {
	_handle.setStyle(SS_ENDELLIPSIS, value);
}


bool Label::focusable() const {
	return false;
}


Size Label::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	auto style = toFlag(_handle.style());
	if (style[SS_BITMAP]) {
		auto bitmap = this->bitmap();
		auto size = bitmap ? bitmap.size() : Size(0, 0);
		size = Scaler().scale(size); // 画像をスケールすべきかどうかは完全にユーザ次第だがここではスケールする、で統一する。
		size = sizeFromClientSize(size);
		return Size(width  ? width  : size.width
				   ,height ? height : size.height);
	} else if (style[SS_ICON]) {
		auto icon = this->icon();
		auto size = icon ? icon.size() : Size(0, 0);
		size = sizeFromClientSize(size);
		return Size(width  ? width  : size.width
				   ,height ? height : size.height);
	} else {
		Graphics graphics(handle());
		auto font = graphics.font(this->font());
		if (!width) {
			width = INT_MAX;
		}
		if (!height) {
			height = INT_MAX;
		}
		auto style = toFlag(_handle.style());
		Graphics::TextFormat format = Graphics::TextFormat::wordBreak;
		if (style[SS_RIGHT       ]) { format |= Graphics::TextFormat::right; }
		if (style[SS_CENTER      ]) { format |= Graphics::TextFormat::horizontalCenter; }
		if (style[SS_EDITCONTROL ]) { format |= Graphics::TextFormat::editControl; }
		if (style[SS_ENDELLIPSIS ]) { format |= Graphics::TextFormat::endEllipsis | Graphics::TextFormat::singleLine; }
		if (style[SS_NOPREFIX    ]) { format |= Graphics::TextFormat::noPrefix; }
		if (style[SS_PATHELLIPSIS]) { format |= Graphics::TextFormat::pathEllipsis | Graphics::TextFormat::singleLine; }
		auto size = graphics.measureText(text(), width, height, format);
		graphics.font(font);
		size = sizeFromClientSize(size);
		return Size(width  != INT_MAX ? width  : size.width
				   ,height != INT_MAX ? height : size.height);
	}
}


Icon Label::icon() const {
	auto icon = (HICON)SendMessageW(handle(), STM_GETICON, 0, 0);
	return Icon(icon);
}


void Label::icon(HICON value) {
	assert("Non icon label" && _handle.hasStyle(SS_ICON));
	SendMessageW(handle(), STM_SETICON, (WPARAM)value, 0);
}


bool Label::noPrefix() const {
	return _handle.hasStyle(SS_NOPREFIX);
}


void Label::noPrefix(bool value) {
	_handle.setStyle(SS_NOPREFIX, value);
}


bool Label::pathEllipsis() const {
	return _handle.hasStyle(SS_PATHELLIPSIS);
}


void Label::pathEllipsis(bool value) {
	_handle.setStyle(SS_PATHELLIPSIS, value);
}


Label::TextAlign Label::textAlign() const {
	return static_cast<TextAlign>(_handle.style() & (SS_LEFT | SS_CENTER | SS_RIGHT));
}


void Label::textAlign(Label::TextAlign value) {
	if (value != textAlign()) {
		auto style = toFlag(_handle.style()).set(SS_LEFT | SS_CENTER | SS_RIGHT, false);
		style.set(value, true);
		_handle.style(style);
		_handle.updateStyle();
	}
}


Color Label::textColor() const {
	return _textColor;
}


void Label::textColor(const Color& value) {
	if (value != _textColor) {
		_textColor = value;
		invalidate();
	}
}


void Label::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_MOVE : {
			Control::processMessage(msg);
			auto parent = this->parent();
			if (!brush() && parent && parent->brush()) { // 親コントロールと地続きな背景を持つので描画しなおし
				invalidate();
			}
		} break;
		case WM_NCHITTEST : { // HTTRANSPARENT を返さないようにする
			Rectangle screenRect(Point(0, 0), size());
			screenRect = clientToScreen(screenRect);
			auto point = Point(GET_X_LPARAM(msg.lparam), GET_Y_LPARAM(msg.lparam));
			msg.result = screenRect.contains(point) ? HTCLIENT : HTNOWHERE;
		} break;
		case wmReflect + WM_CTLCOLORSTATIC : {
			Graphics graphics((HDC)msg.wparam);
			auto textColor = this->textColor();
			HBRUSH brush = this->brush();
			auto origin = brushOrigin();
			if (!brush) { // 親コントロールと地続きな背景を持つので親のブラシがあれば使う
				auto parent = this->parent();
				if (parent) {
					brush = parent->brush();
					origin = parent->displayRectangle().position() + parent->brushOrigin() - position();
				}
			}
			if (brush || textColor != Color::controlText()) {
				graphics.backTransparent(true);
				graphics.textColor(textColor);
				graphics.brushOrigin(origin);
				msg.result = (LRESULT)(brush ? brush : static_cast<HBRUSH>(Brush::control()));
			} else {
				Control::processMessage(msg);
			}
		} break;
		case WM_SIZE : {
			invalidate();
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}