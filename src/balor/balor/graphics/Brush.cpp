#include "Brush.hpp"

#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace graphics {


using std::move;
using std::swap;


namespace {
static_assert(Brush::Hatch::horizontal       == HS_HORIZONTAL, "Invalid enum value");
static_assert(Brush::Hatch::vertical         == HS_VERTICAL, "Invalid enum value");
static_assert(Brush::Hatch::forwardDiagonal  == HS_FDIAGONAL, "Invalid enum value");
static_assert(Brush::Hatch::backwardDiagonal == HS_BDIAGONAL, "Invalid enum value");
static_assert(Brush::Hatch::cross            == HS_CROSS, "Invalid enum value");
static_assert(Brush::Hatch::diagonalCross    == HS_DIAGCROSS, "Invalid enum value");
} // namespace


bool Brush::Hatch::_validate(Hatch value) {
	return value == none || (horizontal <= value && value <= diagonalCross);
}


Brush::Brush() : _handle(nullptr), _owned(false) {
}


Brush::Brush(Brush&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Brush::Brush(HBRUSH handle, bool owned) : _handle(handle), _owned(owned) {
}


Brush::Brush(HBITMAP bitmap) {
	assert("Null bitmap" && bitmap);
	_handle = CreatePatternBrush(bitmap);
	assert("Failed to CreatePatternBrush" && _handle);
	_owned = true;
}


Brush::Brush(const Color& color, Brush::Hatch hatch) {
	assert("Invalid Brush::Hatch" && Hatch::_validate(hatch));
	if (hatch == Hatch::none) {
		_handle = CreateSolidBrush(color.toCOLORREF());
		assert("Failed to CreateSolidBrush" && _handle);
	} else {
		_handle = CreateHatchBrush(hatch, color.toCOLORREF());
		assert("Failed to CreateHatchBrush" && _handle);
	}
	_owned = true;
}


Brush::~Brush() {
	if (_handle && _owned) {
		verify(DeleteObject(_handle)); // ブラシがSelectObjectされたままか、または既に有効なハンドルではない
	}
	//_owned = false;
	//_handle = nullptr;
}


Brush& Brush::operator=(Brush&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


Bitmap Brush::bitmap() const {
	assert("Null Brush handle" && *this);
	LOGBRUSH info;
	verify(GetObjectW(_handle, sizeof(info), &info));
	return info.lbStyle == BS_PATTERN ? Bitmap(reinterpret_cast<HBITMAP>(info.lbHatch)) : Bitmap();
}


Brush Brush::clone() const {
	if (!*this) {
		return Brush();
	}
	return clone(_handle);
}


Brush Brush::clone(HBRUSH handle) {
	assert("Null handle" && handle);
	LOGBRUSH info;
	verify(GetObjectW(handle, sizeof(info), &info));
	Brush brush(CreateBrushIndirect(&info), true);
	assert("Failed to CreateBrushIndirect" && brush);
	return brush;
}


Color Brush::color() const {
	assert("Null Brush handle" && *this);
	LOGBRUSH info;
	verify(GetObjectW(_handle, sizeof(info), &info));
	return Color::fromCOLORREF(info.lbColor);
}


Brush::Hatch Brush::hatch() const {
	assert("Null Brush handle" && *this);
	LOGBRUSH info;
	verify(GetObjectW(_handle, sizeof(info), &info));
	return info.lbStyle == BS_HATCHED ? static_cast<Hatch>(info.lbHatch) : Hatch::none;
}


bool Brush::owned() const {
	return _owned;
}


void Brush::owned(bool value) {
	_owned = value;
}


Brush Brush::white()                   { return Brush(static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH ))); }
Brush Brush::lightGray()               { return Brush(static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH))); }
Brush Brush::darkGray()                { return Brush(static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH))); }
Brush Brush::black()                   { return Brush(static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH ))); }
Brush Brush::hollow()                  { return Brush(static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH))); }

Brush Brush::activeBorder()            { return Brush(GetSysColorBrush(COLOR_ACTIVEBORDER           )); }
Brush Brush::activeCaption()           { return Brush(GetSysColorBrush(COLOR_ACTIVECAPTION          )); }
Brush Brush::activeCaptionText()       { return Brush(GetSysColorBrush(COLOR_CAPTIONTEXT            )); }
Brush Brush::appWorkspace()            { return Brush(GetSysColorBrush(COLOR_APPWORKSPACE           )); }
Brush Brush::control()                 { return Brush(GetSysColorBrush(COLOR_3DFACE                 )); }
Brush Brush::controlShadow()           { return Brush(GetSysColorBrush(COLOR_3DSHADOW               )); }
Brush Brush::controlDarkShadow()       { return Brush(GetSysColorBrush(COLOR_3DDKSHADOW             )); }
Brush Brush::controlLight()            { return Brush(GetSysColorBrush(COLOR_3DLIGHT                )); }
Brush Brush::controlHighlight()        { return Brush(GetSysColorBrush(COLOR_3DHIGHLIGHT            )); }
Brush Brush::controlText()             { return Brush(GetSysColorBrush(COLOR_BTNTEXT                )); }
Brush Brush::desktop()                 { return Brush(GetSysColorBrush(COLOR_DESKTOP                )); }
Brush Brush::gradientActiveCaption()   { return Brush(GetSysColorBrush(COLOR_GRADIENTACTIVECAPTION  )); }
Brush Brush::gradientInactiveCaption() { return Brush(GetSysColorBrush(COLOR_GRADIENTINACTIVECAPTION)); }
Brush Brush::grayText()                { return Brush(GetSysColorBrush(COLOR_GRAYTEXT               )); }
Brush Brush::hotTrack()                { return Brush(GetSysColorBrush(COLOR_HOTLIGHT               )); }
Brush Brush::inactiveBorder()          { return Brush(GetSysColorBrush(COLOR_INACTIVEBORDER         )); }
Brush Brush::inactiveCaption()         { return Brush(GetSysColorBrush(COLOR_INACTIVECAPTION        )); }
Brush Brush::inactiveCaptionText()     { return Brush(GetSysColorBrush(COLOR_INACTIVECAPTIONTEXT    )); }
Brush Brush::menu()                    { return Brush(GetSysColorBrush(COLOR_MENU                   )); }
Brush Brush::menuBar()                 { return Brush(GetSysColorBrush(COLOR_MENUBAR                )); }
Brush Brush::menuHighlight()           { return Brush(GetSysColorBrush(COLOR_MENUHILIGHT            )); }
Brush Brush::menuText()                { return Brush(GetSysColorBrush(COLOR_MENUTEXT               )); }
Brush Brush::scrollbar()               { return Brush(GetSysColorBrush(COLOR_SCROLLBAR              )); }
Brush Brush::selectedControl()         { return Brush(GetSysColorBrush(COLOR_HIGHLIGHT              )); }
Brush Brush::selectedControlText()     { return Brush(GetSysColorBrush(COLOR_HIGHLIGHTTEXT          )); }
Brush Brush::toolTip()                 { return Brush(GetSysColorBrush(COLOR_INFOBK                 )); }
Brush Brush::toolTipText()             { return Brush(GetSysColorBrush(COLOR_INFOTEXT               )); }
Brush Brush::window()                  { return Brush(GetSysColorBrush(COLOR_WINDOW                 )); }
Brush Brush::windowFrame()             { return Brush(GetSysColorBrush(COLOR_WINDOWFRAME            )); }
Brush Brush::windowText()              { return Brush(GetSysColorBrush(COLOR_WINDOWTEXT             )); }



	}
}