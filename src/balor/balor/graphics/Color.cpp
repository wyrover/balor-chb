#include "Color.hpp"

#include <type_traits>

#include <balor/system/windows.hpp>


namespace balor {
	namespace graphics {


namespace {
static_assert(std::is_same<COLORREF, Color::COLORREF>::value, "Invalid typedef");
} // namespace



Color::Color(int r, int g, int b, int a) : a(static_cast<unsigned char>(a)), r(static_cast<unsigned char>(r)), g(static_cast<unsigned char>(g)), b(static_cast<unsigned char>(b)) {
}


Color Color::fromCOLORREF(COLORREF colorref) {
	static_assert(sizeof(Color) == sizeof(COLORREF), "Bad optimize");
	colorref |= 0xff000000;
	return *reinterpret_cast<Color*>(&colorref);
	//return Color(GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
}


COLORREF Color::toCOLORREF() const {
	return *reinterpret_cast<const COLORREF*>(this) & 0x00ffffff;
	//return RGB(r, g, b);
}


bool Color::operator==(const Color& rhs) const {
	static_assert(sizeof(Color) == sizeof(int), "Bad optimize");
	return *reinterpret_cast<const int*>(this) == *reinterpret_cast<const int*>(&rhs);
	//return a == rhs.a && r == rhs.r && g == rhs.g && b == rhs.b;
}


bool Color::operator!=(const Color& rhs) const {
	return !(*this == rhs);
}


Color Color::activeBorder()            { return Color::fromCOLORREF(GetSysColor(COLOR_ACTIVEBORDER           )); }
Color Color::activeCaption()           { return Color::fromCOLORREF(GetSysColor(COLOR_ACTIVECAPTION          )); }
Color Color::activeCaptionText()       { return Color::fromCOLORREF(GetSysColor(COLOR_CAPTIONTEXT            )); }
Color Color::appWorkspace()            { return Color::fromCOLORREF(GetSysColor(COLOR_APPWORKSPACE           )); }
Color Color::control()                 { return Color::fromCOLORREF(GetSysColor(COLOR_3DFACE                 )); }
Color Color::controlShadow()           { return Color::fromCOLORREF(GetSysColor(COLOR_3DSHADOW               )); }
Color Color::controlDarkShadow()       { return Color::fromCOLORREF(GetSysColor(COLOR_3DDKSHADOW             )); }
Color Color::controlLight()            { return Color::fromCOLORREF(GetSysColor(COLOR_3DLIGHT                )); }
Color Color::controlHighlight()        { return Color::fromCOLORREF(GetSysColor(COLOR_3DHIGHLIGHT            )); }
Color Color::controlText()             { return Color::fromCOLORREF(GetSysColor(COLOR_BTNTEXT                )); }
Color Color::desktop()                 { return Color::fromCOLORREF(GetSysColor(COLOR_DESKTOP                )); }
Color Color::gradientActiveCaption()   { return Color::fromCOLORREF(GetSysColor(COLOR_GRADIENTACTIVECAPTION  )); }
Color Color::gradientInactiveCaption() { return Color::fromCOLORREF(GetSysColor(COLOR_GRADIENTINACTIVECAPTION)); }
Color Color::grayText()                { return Color::fromCOLORREF(GetSysColor(COLOR_GRAYTEXT               )); }
Color Color::hotTrack()                { return Color::fromCOLORREF(GetSysColor(COLOR_HOTLIGHT               )); }
Color Color::inactiveBorder()          { return Color::fromCOLORREF(GetSysColor(COLOR_INACTIVEBORDER         )); }
Color Color::inactiveCaption()         { return Color::fromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION        )); }
Color Color::inactiveCaptionText()     { return Color::fromCOLORREF(GetSysColor(COLOR_INACTIVECAPTIONTEXT    )); }
Color Color::menu()                    { return Color::fromCOLORREF(GetSysColor(COLOR_MENU                   )); }
Color Color::menuBar()                 { return Color::fromCOLORREF(GetSysColor(COLOR_MENUBAR                )); }
Color Color::menuHighlight()           { return Color::fromCOLORREF(GetSysColor(COLOR_MENUHILIGHT            )); }
Color Color::menuText()                { return Color::fromCOLORREF(GetSysColor(COLOR_MENUTEXT               )); }
Color Color::scrollbar()               { return Color::fromCOLORREF(GetSysColor(COLOR_SCROLLBAR              )); }
Color Color::selectedControl()         { return Color::fromCOLORREF(GetSysColor(COLOR_HIGHLIGHT              )); }
Color Color::selectedControlText()     { return Color::fromCOLORREF(GetSysColor(COLOR_HIGHLIGHTTEXT          )); }
Color Color::toolTip()                 { return Color::fromCOLORREF(GetSysColor(COLOR_INFOBK                 )); }
Color Color::toolTipText()             { return Color::fromCOLORREF(GetSysColor(COLOR_INFOTEXT               )); }
Color Color::window()                  { return Color::fromCOLORREF(GetSysColor(COLOR_WINDOW                 )); }
Color Color::windowFrame()             { return Color::fromCOLORREF(GetSysColor(COLOR_WINDOWFRAME            )); }
Color Color::windowText()              { return Color::fromCOLORREF(GetSysColor(COLOR_WINDOWTEXT             )); }


// 色名一覧。この URL を参考。http://msdn.microsoft.com/en-us/library/aa358802(v=vs.85).aspx
Color Color::aliceBlue()            { return fromCOLORREF(0xFFFFF8F0); }
Color Color::antiqueWhite()         { return fromCOLORREF(0xFFD7EBFA); }
Color Color::aqua()                 { return fromCOLORREF(0xFFFFFF00); }
Color Color::aquamarine()           { return fromCOLORREF(0xFFD4FF7F); }
Color Color::azure()                { return fromCOLORREF(0xFFFFFFF0); }
Color Color::beige()                { return fromCOLORREF(0xFFDCF5F5); }
Color Color::bisque()               { return fromCOLORREF(0xFFC4E4FF); }
Color Color::black()                { return fromCOLORREF(0xFF000000); }
Color Color::blanchedAlmond()       { return fromCOLORREF(0xFFCDEBFF); }
Color Color::blue()                 { return fromCOLORREF(0xFFFF0000); }
Color Color::blueViolet()           { return fromCOLORREF(0xFFE22B8A); }
Color Color::brown()                { return fromCOLORREF(0xFF2A2AA5); }
Color Color::burlyWood()            { return fromCOLORREF(0xFF87B8DE); }
Color Color::cadetBlue()            { return fromCOLORREF(0xFFA09E5F); }
Color Color::chartreuse()           { return fromCOLORREF(0xFF00FF7F); }
Color Color::chocolate()            { return fromCOLORREF(0xFF1E69D2); }
Color Color::coral()                { return fromCOLORREF(0xFF507FFF); }
Color Color::cornflowerBlue()       { return fromCOLORREF(0xFFED9564); }
Color Color::cornsilk()             { return fromCOLORREF(0xFFDCF8FF); }
Color Color::crimson()              { return fromCOLORREF(0xFF3C14DC); }
Color Color::cyan()                 { return fromCOLORREF(0xFFFFFF00); }
Color Color::darkBlue()             { return fromCOLORREF(0xFF8B0000); }
Color Color::darkCyan()             { return fromCOLORREF(0xFF8B8B00); }
Color Color::darkGoldenrod()        { return fromCOLORREF(0xFF0B86B8); }
Color Color::darkGray()             { return fromCOLORREF(0xFFA9A9A9); }
Color Color::darkGreen()            { return fromCOLORREF(0xFF006400); }
Color Color::darkKhaki()            { return fromCOLORREF(0xFF6BB7BD); }
Color Color::darkMagenta()          { return fromCOLORREF(0xFF8B008B); }
Color Color::darkOliveGreen()       { return fromCOLORREF(0xFF2F6B55); }
Color Color::darkOrange()           { return fromCOLORREF(0xFF008CFF); }
Color Color::darkOrchid()           { return fromCOLORREF(0xFFCC3299); }
Color Color::darkRed()              { return fromCOLORREF(0xFF00008B); }
Color Color::darkSalmon()           { return fromCOLORREF(0xFF7A96E9); }
Color Color::darkSeaGreen()         { return fromCOLORREF(0xFF8FBC8F); }
Color Color::darkSlateBlue()        { return fromCOLORREF(0xFF8B3D48); }
Color Color::darkSlateGray()        { return fromCOLORREF(0xFF4F4F2F); }
Color Color::darkTurquoise()        { return fromCOLORREF(0xFFD1CE00); }
Color Color::darkViolet()           { return fromCOLORREF(0xFFD30094); }
Color Color::deepPink()             { return fromCOLORREF(0xFF9314FF); }
Color Color::deepSkyBlue()          { return fromCOLORREF(0xFFFFBF00); }
Color Color::dimGray()              { return fromCOLORREF(0xFF696969); }
Color Color::dodgerBlue()           { return fromCOLORREF(0xFFFF901E); }
Color Color::fireBrick()            { return fromCOLORREF(0xFF2222B2); }
Color Color::floralWhite()          { return fromCOLORREF(0xFFF0FAFF); }
Color Color::forestGreen()          { return fromCOLORREF(0xFF228B22); }
Color Color::fuchsia()              { return fromCOLORREF(0xFFFF00FF); }
Color Color::gainsboro()            { return fromCOLORREF(0xFFDCDCDC); }
Color Color::ghostWhite()           { return fromCOLORREF(0xFFFFF8F8); }
Color Color::gold()                 { return fromCOLORREF(0xFF00D7FF); }
Color Color::goldenrod()            { return fromCOLORREF(0xFF20A5DA); }
Color Color::gray()                 { return fromCOLORREF(0xFF808080); }
Color Color::green()                { return fromCOLORREF(0xFF008000); }
Color Color::greenYellow()          { return fromCOLORREF(0xFF2FFFAD); }
Color Color::honeydew()             { return fromCOLORREF(0xFFF0FFF0); }
Color Color::hotPink()              { return fromCOLORREF(0xFFB469FF); }
Color Color::indianRed()            { return fromCOLORREF(0xFF5C5CCD); }
Color Color::indigo()               { return fromCOLORREF(0xFF82004B); }
Color Color::ivory()                { return fromCOLORREF(0xFFF0FFFF); }
Color Color::khaki()                { return fromCOLORREF(0xFF8CE6F0); }
Color Color::lavender()             { return fromCOLORREF(0xFFFAE6E6); }
Color Color::lavenderBlush()        { return fromCOLORREF(0xFFF5F0FF); }
Color Color::lawnGreen()            { return fromCOLORREF(0xFF00FC7C); }
Color Color::lemonChiffon()         { return fromCOLORREF(0xFFCDFAFF); }
Color Color::lightBlue()            { return fromCOLORREF(0xFFE6D8AD); }
Color Color::lightCoral()           { return fromCOLORREF(0xFF8080F0); }
Color Color::lightCyan()            { return fromCOLORREF(0xFFFFFFE0); }
Color Color::lightGoldenrodYellow() { return fromCOLORREF(0xFFD2FAFA); }
Color Color::lightGreen()           { return fromCOLORREF(0xFF90EE90); }
Color Color::lightGray()            { return fromCOLORREF(0xFFD3D3D3); }
Color Color::lightPink()            { return fromCOLORREF(0xFFC1B6FF); }
Color Color::lightSalmon()          { return fromCOLORREF(0xFF7AA0FF); }
Color Color::lightSeaGreen()        { return fromCOLORREF(0xFFAAB220); }
Color Color::lightSkyBlue()         { return fromCOLORREF(0xFFFACE87); }
Color Color::lightSlateGray()       { return fromCOLORREF(0xFF998877); }
Color Color::lightSteelBlue()       { return fromCOLORREF(0xFFDEC4B0); }
Color Color::lightYellow()          { return fromCOLORREF(0xFFE0FFFF); }
Color Color::lime()                 { return fromCOLORREF(0xFF00FF00); }
Color Color::limeGreen()            { return fromCOLORREF(0xFF32CD32); }
Color Color::linen()                { return fromCOLORREF(0xFFE6F0FA); }
Color Color::magenta()              { return fromCOLORREF(0xFFFF00FF); }
Color Color::maroon()               { return fromCOLORREF(0xFF000080); }
Color Color::mediumAquamarine()     { return fromCOLORREF(0xFFAACD66); }
Color Color::mediumBlue()           { return fromCOLORREF(0xFFCD0000); }
Color Color::mediumOrchid()         { return fromCOLORREF(0xFFD355BA); }
Color Color::mediumPurple()         { return fromCOLORREF(0xFFDB7093); }
Color Color::mediumSeaGreen()       { return fromCOLORREF(0xFF71B33C); }
Color Color::mediumSlateBlue()      { return fromCOLORREF(0xFFEE687B); }
Color Color::mediumSpringGreen()    { return fromCOLORREF(0xFF9AFA00); }
Color Color::mediumTurquoise()      { return fromCOLORREF(0xFFCCD148); }
Color Color::mediumVioletRed()      { return fromCOLORREF(0xFF8515C7); }
Color Color::midnightBlue()         { return fromCOLORREF(0xFF701919); }
Color Color::mintCream()            { return fromCOLORREF(0xFFFAFFF5); }
Color Color::mistyRose()            { return fromCOLORREF(0xFFE1E4FF); }
Color Color::moccasin()             { return fromCOLORREF(0xFFB5E4FF); }
Color Color::navajoWhite()          { return fromCOLORREF(0xFFADDEFF); }
Color Color::navy()                 { return fromCOLORREF(0xFF800000); }
Color Color::oldLace()              { return fromCOLORREF(0xFFE6F5FD); }
Color Color::olive()                { return fromCOLORREF(0xFF008080); }
Color Color::oliveDrab()            { return fromCOLORREF(0xFF238E6B); }
Color Color::orange()               { return fromCOLORREF(0xFF00A5FF); }
Color Color::orangeRed()            { return fromCOLORREF(0xFF0045FF); }
Color Color::orchid()               { return fromCOLORREF(0xFFD670DA); }
Color Color::paleGoldenrod()        { return fromCOLORREF(0xFFAAE8EE); }
Color Color::paleGreen()            { return fromCOLORREF(0xFF98FB98); }
Color Color::paleTurquoise()        { return fromCOLORREF(0xFFEEEEAF); }
Color Color::paleVioletRed()        { return fromCOLORREF(0xFF9370DB); }
Color Color::papayaWhip()           { return fromCOLORREF(0xFFD5EFFF); }
Color Color::peachPuff()            { return fromCOLORREF(0xFFB9DAFF); }
Color Color::peru()                 { return fromCOLORREF(0xFF3F85CD); }
Color Color::pink()                 { return fromCOLORREF(0xFFCBC0FF); }
Color Color::plum()                 { return fromCOLORREF(0xFFDDA0DD); }
Color Color::powderBlue()           { return fromCOLORREF(0xFFE6E0B0); }
Color Color::purple()               { return fromCOLORREF(0xFF800080); }
Color Color::red()                  { return fromCOLORREF(0xFF0000FF); }
Color Color::rosyBrown()            { return fromCOLORREF(0xFF8F8FBC); }
Color Color::royalBlue()            { return fromCOLORREF(0xFFE16941); }
Color Color::saddleBrown()          { return fromCOLORREF(0xFF13458B); }
Color Color::salmon()               { return fromCOLORREF(0xFF7280FA); }
Color Color::sandyBrown()           { return fromCOLORREF(0xFF60A4F4); }
Color Color::seaGreen()             { return fromCOLORREF(0xFF578B2E); }
Color Color::seashell()             { return fromCOLORREF(0xFFEEF5FF); }
Color Color::sienna()               { return fromCOLORREF(0xFF2D52A0); }
Color Color::silver()               { return fromCOLORREF(0xFFC0C0C0); }
Color Color::skyBlue()              { return fromCOLORREF(0xFFEBCE87); }
Color Color::slateBlue()            { return fromCOLORREF(0xFFCD5A6A); }
Color Color::slateGray()            { return fromCOLORREF(0xFF908070); }
Color Color::snow()                 { return fromCOLORREF(0xFFFAFAFF); }
Color Color::springGreen()          { return fromCOLORREF(0xFF7FFF00); }
Color Color::steelBlue()            { return fromCOLORREF(0xFFB48246); }
Color Color::tan()                  { return fromCOLORREF(0xFF8CB4D2); }
Color Color::teal()                 { return fromCOLORREF(0xFF808000); }
Color Color::thistle()              { return fromCOLORREF(0xFFD8BFD8); }
Color Color::tomato()               { return fromCOLORREF(0xFF4763FF); }
Color Color::turquoise()            { return fromCOLORREF(0xFFD0E040); }
Color Color::violet()               { return fromCOLORREF(0xFFEE82EE); }
Color Color::wheat()                { return fromCOLORREF(0xFFB3DEF5); }
Color Color::white()                { return fromCOLORREF(0xFFFFFFFF); }
Color Color::whiteSmoke()           { return fromCOLORREF(0xFFF5F5F5); }
Color Color::yellow()               { return fromCOLORREF(0xFF00FFFF); }
Color Color::yellowGreen()          { return fromCOLORREF(0xFF32CD9A); }



	}
}