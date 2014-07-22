#include "GroupBox.hpp"

#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Scaler.hpp>
#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <WindowsX.h>


namespace balor {
	namespace gui {

using std::move;
using namespace balor::graphics;



GroupBox::GroupBox() {
}


GroupBox::GroupBox(Control& parent, int x, int y, int width, int height, StringRange text) {
	attachHandle(CreateWindowExW(0, L"BUTTON", text.c_str()
		, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | BS_GROUPBOX
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
}


GroupBox::GroupBox(GroupBox&& value, bool checkSlicing)
	: Control(move(value))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


GroupBox::~GroupBox() {
}


GroupBox& GroupBox::operator=(GroupBox&& value) {
	if (&value != this) {
		this->~GroupBox();
		new (this) GroupBox(move(value));
	}
	return *this;
}


Brush GroupBox::brush() const {
	auto brush = Control::brush();
	if (brush) {
		return brush;
	}
	auto parent = this->parent();
	if (!parent) {
		return Brush();
	}
	return parent->brush(); // 親のブラシを使って描画するので子にも引き継ぐ
}


Point GroupBox::brushOrigin() const {
	auto brush = Control::brush();
	if (brush) {
		return Control::brushOrigin();
	}
	auto parent = this->parent();
	if (!parent) {
		return Control::brushOrigin();
	}
	return parent->displayRectangle().position() + parent->brushOrigin() - position();
}


Rectangle GroupBox::displayRectangle() const {
	auto size = clientSize();
	auto fontHeight = font().height();
	return Rectangle(0, fontHeight, size.width, size.height - fontHeight);
}


Size GroupBox::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	auto size = Control::getPreferredSize(width, height);
	size.height += font().height();
	size += Scaler().scale(Size(4, 4));
	return size;
}


bool GroupBox::focusable() const {
	return false; // 方向キーでフォーカスを得ないように上書き。
}


void GroupBox::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ERASEBKGND : {
			Graphics graphics((HDC)msg.wparam);
			auto brush = this->brush();
			if (brush) {
				brush = graphics.brush(brush);
				auto origin = graphics.brushOrigin(brushOrigin());
				graphics.clear(); // GroupBox で描画するのをやめて WS_EX_TRANSPARENT スタイルにするとGroupBox のリサイズ時に子もふくめて激しくちらつく
				graphics.brushOrigin(origin);
			} else {
				brush = graphics.brush(Brush::control());
				graphics.clear();
			}
			graphics.brush(brush);
			msg.result = TRUE;
		} break;
		case WM_LBUTTONDBLCLK : { // Button と同じ挙動をしようとしてフォーカスを得たりするので防ぐ
			MouseDoubleClick event(*this, Mouse::lButton , msg);
			onMouseDoubleClick()(event);
		} break;
		case WM_LBUTTONDOWN : { // Button と同じ挙動をしようとしてフォーカスを得たりするので防ぐ
			processMouseDown(MouseDown(*this, Mouse::lButton , msg, _dragBox));
		} break;
		case WM_MOVE : {
			Control::processMessage(msg);
			auto parent = this->parent();
			if (parent && parent->brush()) { // 親コントロールと地続きな背景を持つので描画しなおし
				invalidate();
			}
		} break;
		case WM_NCHITTEST : { // HTTRANSPARENT を返さないようにする
			Rectangle screenRect(Point(0, 0), size());
			screenRect = clientToScreen(screenRect);
			auto point = Point(GET_X_LPARAM(msg.lparam), GET_Y_LPARAM(msg.lparam));
			msg.result = screenRect.contains(point) ? HTCLIENT : HTNOWHERE;
		} break;
		case wmReflect + WM_CTLCOLORSTATIC : { // 文字の背景はここで返したブラシで塗られる
			HBRUSH brush = this->brush();
			if (brush) {
				Graphics graphics((HDC)msg.wparam);
				graphics.brushOrigin(brushOrigin());
				msg.result = (LRESULT)brush;
			} else {
				Control::processMessage(msg);
			}
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}