#include "ColorDialog.hpp"

#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommDlg.h>
#include <CdErr.h>
#include <Dlgs.h>


namespace balor {
	namespace gui {

using std::move;
using namespace balor::graphics;



ColorDialog::Event::Event(HWND sender)
	: _sender(sender) {
}


HWND ColorDialog::Event::sender() const {
	return _sender;
}



ColorDialog::Init::Init(HWND sender)
	: ColorDialog::Event(sender) {
}


Point ColorDialog::Init::position() const {
	RECT rect = {0};
	verify(GetWindowRect(sender(), &rect));
	return Point(rect.left, rect.top);
}


void ColorDialog::Init::position(const Point& value) {
	verify(SetWindowPos(sender(), nullptr, value.x, value.y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE));
}


void ColorDialog::Init::position(int x, int y) {
	position(Point(x, y));
}


Size ColorDialog::Init::clientSize() const {
	RECT rect = {0};
	verify(GetClientRect(sender(), &rect));
	return Size(rect.right - rect.left, rect.bottom - rect.top);
}


void ColorDialog::Init::clientSize(const Size& value) {
	RECT rect = {0, 0, value.width, value.height};
	verify(AdjustWindowRectEx(&rect,  GetWindowLongPtrW(sender(), GWL_STYLE), FALSE, GetWindowLongPtrW(sender(), GWL_EXSTYLE)));
	verify(SetWindowPos(sender(), nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE));
}


void ColorDialog::Init::clientSize(int width, int height) {
	clientSize(Size(width, height));
}



ColorDialog::ColorDialog()
	: _color(Color::black())
	, _flags(CC_RGBINIT | CC_ANYCOLOR | CC_FULLOPEN | CC_ENABLEHOOK)
	{
	for (auto i = 0; i < 16; ++i) {
		_customColors[i] = Color::black();
	}
}


ColorDialog::ColorDialog(ColorDialog&& value)
	: _color(move(value._color))
	, _onInit(move(value._onInit))
	, _flags(move(value._flags))
	{
	::memcpy(_customColors, value._customColors, sizeof(_customColors));
}


ColorDialog::~ColorDialog() {
}


Color ColorDialog::color() const {
	return _color;
}


void ColorDialog::color(const Color& value) {
	_color = value;
}


bool ColorDialog::createColorUI() const {
	return toFlag(_flags)[CC_FULLOPEN];
}


void ColorDialog::createColorUI(bool value) {
	_flags = toFlag(_flags).set(CC_FULLOPEN, value);
}


Color ColorDialog::getCustomColor(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < 16);
	return _customColors[index];
}


Listener<ColorDialog::Init&>& ColorDialog::onInit() { return _onInit; }


void ColorDialog::setCustomColor(int index, const Color& value) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < 16);
	_customColors[index] = value;
}


bool ColorDialog::show(HWND owner) {
	COLORREF colors[16] = {0};
	for (auto i = 0; i < 16; ++i) {
		colors[i] = _customColors[i].toCOLORREF();
	}

	CHOOSECOLORW info;
	ZeroMemory(&info, sizeof(info));
	info.lStructSize = sizeof(info);
	info.hwndOwner = owner;
	info.lCustData = (LPARAM)this;
	info.Flags = _flags;
	info.lpCustColors = colors;
	info.rgbResult = _color.toCOLORREF();

	struct Fook {
		static UINT_PTR CALLBACK procedure(HWND handle, UINT message, WPARAM , LPARAM lparam) {
			switch (message) {
				case WM_INITDIALOG : {
					auto info = (CHOOSECOLORW*)lparam;
					ColorDialog* dialog = (ColorDialog*)info->lCustData;
					Init event(handle);
					dialog->onInit()(event);
				} break;
			}
			return 0;
		}
	};
	info.lpfnHook = Fook::procedure;

	auto result = ChooseColorW(&info);
	assert("Failed to ChooseColorW" && (result || !CommDlgExtendedError()));
	if (result) {
		_color = Color::fromCOLORREF(info.rgbResult);
		for (auto i = 0; i < 16; ++i) {
			_customColors[i] = Color::fromCOLORREF(colors[i]);
		}
	}

	return result != 0;
}



	}
}