#include "Mouse.hpp"

#include <balor/graphics/Cursor.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/Size.hpp>


namespace balor {
	namespace gui {

using namespace balor::graphics;


static_assert(Mouse::lButton  == VK_LBUTTON, "Invalid enum value");
static_assert(Mouse::mButton  == VK_MBUTTON, "Invalid enum value");
static_assert(Mouse::rButton  == VK_RBUTTON, "Invalid enum value");
static_assert(Mouse::xButton1 == VK_XBUTTON1, "Invalid enum value");
static_assert(Mouse::xButton2 == VK_XBUTTON2, "Invalid enum value");


bool Mouse::_validate(Mouse value) {
	return none <= value && value <= xButton2 && value != 3;
}


int Mouse::buttonCount() {
	return GetSystemMetrics(SM_CMOUSEBUTTONS);
}


bool Mouse::buttonSwapped() {
	return GetSystemMetrics(SM_SWAPBUTTON) != 0;
}


Rectangle Mouse::clip() {
	RECT rect;
	verify(GetClipCursor(&rect));
	return Rectangle(rect);
}


void Mouse::clip(const Rectangle& value) {
	const RECT rect = value;
	verify(ClipCursor(&rect));
}


Cursor Mouse::cursor() {
	return Cursor(GetCursor());
}


void Mouse::cursor(HCURSOR value) {
	SetCursor(value);
}


Size Mouse::doubleClickSize() {
	return Size(GetSystemMetrics(SM_CXDOUBLECLK), GetSystemMetrics(SM_CYDOUBLECLK));
}


int Mouse::doubleClickTime() {
	return GetDoubleClickTime();
}


void Mouse::hide() {
	ShowCursor(FALSE);
	//for (int count; 0 <= (count = ShowMouse(FALSE)); ) {
	//}
}


Size Mouse::hoverSize() {
	UINT width  = 0;
	verify(SystemParametersInfoW(SPI_GETMOUSEHOVERWIDTH , 0, &width , 0));
	UINT height = 0;
	verify(SystemParametersInfoW(SPI_GETMOUSEHOVERHEIGHT, 0, &height, 0));
	return Size(width, height);
}


int Mouse::hoverTime() {
	UINT result = 0;
	verify(SystemParametersInfoW(SPI_GETMOUSEHOVERTIME, 0, &result, 0));
	return result;
}


bool Mouse::isPushed(Mouse button) {
	assert("Invalid Mouse button" && Mouse::_validate(button) && button != Mouse::none);
	return GetKeyState(button) < 0;
}


bool Mouse::isPushedAsync(Mouse button) {
	assert("Invalid Mouse button" && Mouse::_validate(button) && button != Mouse::none);
	return GetAsyncKeyState(button) < 0;
}


Point Mouse::position() {
	POINT point;
	verify(GetCursorPos(&point));
	return Point(point);
}


void Mouse::position(const Point& value) {
	verify(SetCursorPos(value.x, value.y));
}


Point Mouse::positionAtLastMessage() {
	auto pos = GetMessagePos();
	POINTS points(MAKEPOINTS(pos));
	return Point(points.x, points.y);
}


bool Mouse::present() {
	return GetSystemMetrics(SM_MOUSEPRESENT) != 0;
}


void Mouse::show() {
	ShowCursor(TRUE);
	//for (int count; (count = ShowMouse(TRUE)) < 0; ) {
	//}
}


int Mouse::speed() {
	INT result = 0;
	verify(SystemParametersInfoW(SPI_GETMOUSESPEED, 0, &result, 0));
	return result;
}


bool Mouse::wheelPresent() {
	return GetSystemMetrics(SM_MOUSEWHEELPRESENT) != 0; // XP以降なら必ずサポートされるはず
}


int Mouse::wheelScrollDelta() {
	return WHEEL_DELTA; // 情報取得関数には見当たらない。これであってるか？
}


int Mouse::wheelScrollLines() {
	UINT result = 0;
	verify(SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &result, 0));
	return result;
}



	}
}