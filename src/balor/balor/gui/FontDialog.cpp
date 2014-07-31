#include "FontDialog.hpp"

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



FontDialog::Event::Event(HWND sender)
	: _sender(sender) {
}


HWND FontDialog::Event::sender() const {
	return _sender;
}



FontDialog::Init::Init(HWND sender)
	: FontDialog::Event(sender) {
}


Point FontDialog::Init::position() const {
	RECT rect = {0};
	verify(GetWindowRect(sender(), &rect));
	return Point(rect.left, rect.top);
}


void FontDialog::Init::position(const Point& value) {
	verify(SetWindowPos(sender(), nullptr, value.x, value.y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE));
}


void FontDialog::Init::position(int x, int y) {
	position(Point(x, y));
}


Size FontDialog::Init::clientSize() const {
	RECT rect = {0};
	verify(GetClientRect(sender(), &rect));
	return Size(rect.right - rect.left, rect.bottom - rect.top);
}


void FontDialog::Init::clientSize(const Size& value) {
	RECT rect = {0, 0, value.width, value.height};
	verify(AdjustWindowRectEx(&rect,  GetWindowLongPtrW(sender(), GWL_STYLE), FALSE, GetWindowLongPtrW(sender(), GWL_EXSTYLE)));
	verify(SetWindowPos(sender(), nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE));
}


void FontDialog::Init::clientSize(int width, int height) {
	clientSize(Size(width, height));
}



FontDialog::FontDialog()
	: _color(Color::black())
	, _flags(CF_TTONLY | CF_SCREENFONTS | CF_FORCEFONTEXIST | CF_ENABLEHOOK | CF_EFFECTS)
	{
}


FontDialog::FontDialog(FontDialog&& value)
	: _font(move(value._font))
	, _color(move(value._color))
	, _onInit(move(value._onInit))
	, _flags(move(value._flags))
	{
}


FontDialog::~FontDialog() {
}


bool FontDialog::charsetUI() const {
	return !toFlag(_flags)[CF_NOSCRIPTSEL];
}


void FontDialog::charsetUI(bool value) {
	_flags = toFlag(_flags).set(CF_NOSCRIPTSEL, !value);
}


Color FontDialog::color() const {
	return _color;
}


void FontDialog::color(const Color& value) {
	_color = value;
}


bool FontDialog::effectsUI() const {
	return toFlag(_flags)[CF_EFFECTS];
}


void FontDialog::effectsUI(bool value) {
	_flags = toFlag(_flags).set(CF_EFFECTS, value);
}


bool FontDialog::fixedPitchOnly() const {
	return toFlag(_flags)[CF_FIXEDPITCHONLY];
}


void FontDialog::fixedPitchOnly(bool value) {
	_flags = toFlag(_flags).set(CF_FIXEDPITCHONLY, value);
}


const Font& FontDialog::font() const {
	return _font;
}


void FontDialog::font(HFONT value) {
	_font = Font::clone(value);
}


Listener<FontDialog::Init&>& FontDialog::onInit() { return _onInit; }


bool FontDialog::scriptsOnly() const {
	return !toFlag(_flags)[CF_SCRIPTSONLY];
}


void FontDialog::scriptsOnly(bool value) {
	_flags = toFlag(_flags).set(CF_SCRIPTSONLY, !value);
}


bool FontDialog::show(HWND owner) {
	CHOOSEFONTW info;
	ZeroMemory(&info, sizeof(info));
	info.lStructSize = sizeof(info);
	info.hwndOwner = owner;
	info.lCustData = (LPARAM)this;
	info.Flags = _flags;

	LOGFONTW fontInfo;
	ZeroMemory(&fontInfo, sizeof(fontInfo));
	if (font()) {
		info.Flags |= CF_INITTOLOGFONTSTRUCT;
		verify(GetObjectW((HFONT)font(), sizeof(LOGFONTW), &fontInfo));
	}
	info.lpLogFont = &fontInfo;
	info.rgbColors = _color.toCOLORREF();

	struct Fook {
		static UINT_PTR CALLBACK procedure(HWND handle, UINT message, WPARAM , LPARAM lparam) {
			switch (message) {
				case WM_INITDIALOG : {
					auto info = (CHOOSEFONTW*)lparam;
					FontDialog* dialog = (FontDialog*)info->lCustData;
					Init event(handle);
					dialog->onInit()(event);
				} break;
			}
			return 0;
		}
	};
	info.lpfnHook = Fook::procedure;

	auto result = ChooseFontW(&info);
	assert("Failed to ChooseFontW" && (result || !CommDlgExtendedError()));
	if (result) {
		if (fontInfo.lfQuality == Font::Quality::default
		 || fontInfo.lfQuality == Font::Quality::draft) {
			fontInfo.lfQuality = Font::Quality::proof; // Font クラスのデフォルトは Font::Quality::proof なので補正する
		}
		_font = Font::fromLogFont(fontInfo);
		_color = Color::fromCOLORREF(info.rgbColors);
	}

	return result != 0;
}


bool FontDialog::verticalFonts() const {
	return !toFlag(_flags)[CF_NOVERTFONTS];
}


void FontDialog::verticalFonts(bool value) {
	_flags = toFlag(_flags).set(CF_NOVERTFONTS, !value);
}



	}
}