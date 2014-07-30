#include "Font.hpp"

#include <utility>
#include <vector>

#include <balor/graphics/Graphics.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace graphics {


using std::move;
using std::swap;
using std::vector;


namespace {
static_assert(Font::Quality::default          == DEFAULT_QUALITY, "Invalid enum value");
static_assert(Font::Quality::draft            == DRAFT_QUALITY, "Invalid enum value");
static_assert(Font::Quality::proof            == PROOF_QUALITY, "Invalid enum value");
static_assert(Font::Quality::nonantialiased   == NONANTIALIASED_QUALITY, "Invalid enum value");
static_assert(Font::Quality::antialiased      == ANTIALIASED_QUALITY, "Invalid enum value");
static_assert(Font::Quality::clearType        == CLEARTYPE_QUALITY, "Invalid enum value");
//static_assert(Font::Quality::clearTypeNatural == CLEARTYPE_NATURAL_QUALITY, "Invalid enum value");


struct FontInfo : public LOGFONTW {
	FontInfo(HFONT handle) {
		assert("Null Font handle" && handle);
		verify(GetObjectW(handle, sizeof(LOGFONTW), this));
	}
};
} // namespace


bool Font::Style::_validate(Style value) {
	return (value & ~(
		  regular  
		| bold     
		| italic   
		| strikeout
		| underline
		)) == 0; 
}


bool Font::Quality::_validate(Quality value) {
	return default <= value && value <= clearType;
}


Font::Font() : _handle(nullptr), _owned(false) {
}


Font::Font(Font&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Font::Font(HFONT handle, bool owned) : _handle(handle), _owned(owned) {
}


Font::Font(const Font& font, int height, Font::Style style) : _handle(nullptr), _owned(false) {
	assert("Null font handle" && font);
	assert("Invalid Font::Style" && Style::_validate(style));

	FontInfo info(font);
	info.lfWeight    = toFlag(style)[Font::Style::bold     ] ? FW_BOLD : FW_REGULAR;
	info.lfItalic    = toFlag(style)[Font::Style::italic   ] ? TRUE : FALSE;
	info.lfStrikeOut = toFlag(style)[Font::Style::strikeout] ? TRUE : FALSE;
	info.lfUnderline = toFlag(style)[Font::Style::underline] ? TRUE : FALSE;
	info.lfHeight = -height;
	_handle = CreateFontIndirectW(&info);
	assert("Failed to CreateFondIndirect" && _handle);
	_owned = true;
}


Font::Font(StringRange name, int height, Font::Style style, Font::Quality quality, int escapement, int orientation) : _handle(nullptr), _owned(false) {
	assert("Empty name" && !name.empty());
	assert("height less than 0" && 0 < height);
	assert("Invalid Font::Style" && Style::_validate(style));
	assert("Invalid Font::Quality" && Quality::_validate(quality));
	assert("escapement out of range" && -3600 <= escapement);
	assert("escapement out of range" && escapement <= 3600);
	assert("orientation out of range" && -3600 <= orientation);
	assert("orientation out of range" && orientation <= 3600);

	const int weight     = toFlag(style)[Font::Style::bold     ] ? FW_BOLD : FW_REGULAR;
	const BOOL italic    = toFlag(style)[Font::Style::italic   ] ? TRUE : FALSE;
	const BOOL strikeout = toFlag(style)[Font::Style::strikeout] ? TRUE : FALSE;
	const BOOL underline = toFlag(style)[Font::Style::underline] ? TRUE : FALSE;
	_handle = CreateFontW(-height, 0, escapement, orientation, weight, italic, underline, strikeout
		, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, quality, DEFAULT_PITCH | FF_DONTCARE
		, name.c_str());
	assert("Failed to CreateFondIndirect" && _handle);
	_owned = true;
}


Font::~Font() {
	if (_handle && _owned) {
		verify(DeleteObject(_handle));
	}
	//_owned = false;
	//_handle = nullptr;
}


Font& Font::operator=(Font&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


int Font::averageWidth() const {
	Graphics graphics = Graphics::fromGraphics(nullptr);
	auto font = graphics.font(*this);
	TEXTMETRICW metric;
	verify(GetTextMetricsW(graphics, &metric));
	graphics.font(font);
	return metric.tmAveCharWidth;
}


Font Font::clone() const {
	if (!*this) {
		return Font();
	}
	return clone(*this);
}


Font Font::clone(HFONT handle) {
	assert("Null handle" && handle);
	FontInfo info(handle);
	Font font(CreateFontIndirectW(&info), true);
	assert("Failed to CreateFondIndirect" && font);
	return font;
}


int Font::escapement() const {
	return FontInfo(*this).lfEscapement;
}


Font Font::fromLogFont(const LOGFONTW& logfont) {
	Font font(CreateFontIndirectW(&logfont), true);
	assert("Failed to CreateFondIndirect" && font);
	return font;
}


int Font::height() const {
	return std::abs(FontInfo(*this).lfHeight);
}


String Font::name() const {
	assert("Null Font handle" && *this);

	//return FontInfo(*this).lfFaceName; // これだと本当の名前はわからない

	Graphics graphics = Graphics::fromGraphics(nullptr);
	auto font = graphics.font(*this);
	WCHAR faceName[LF_FACESIZE];
	verify(GetTextFaceW(graphics, sizeof(faceName) / sizeof(faceName[0]), faceName));
	graphics.font(font);
	return faceName;
}


vector<String> Font::names() {
	struct FontFamilies {
		static int CALLBACK procedure(const LOGFONTW* logfont, const TEXTMETRICW* , DWORD fontType, LPARAM lparam) {
			if ((fontType & TRUETYPE_FONTTYPE) == TRUETYPE_FONTTYPE) { // true type 만
				auto& names = *reinterpret_cast<vector<String>*>(lparam);
				names.push_back(String(logfont->lfFaceName));
			}
			return TRUE;
		}
	};
	vector<String> result;
	EnumFontFamiliesW(Graphics::fromScreen(), nullptr, FontFamilies::procedure, (LPARAM)&result);
	return result;
}


int Font::orientation() const {
	return FontInfo(*this).lfOrientation;
}


bool Font::owned() const {
	return _owned;
}


void Font::owned(bool value) {
	_owned = value;
}


Font::Quality Font::quality() const {
	return static_cast<Font::Quality>(FontInfo(*this).lfQuality);
}


Font::Style Font::style() const {
	assert("Null Font handle" && *this);

	FontInfo info(*this);
	Font::Style result = Style::regular;
	if (info.lfWeight == FW_BOLD) {
		result |= Font::Style::bold;
	}
	if (info.lfItalic) {
		result |= Font::Style::italic;
	}
	if (info.lfStrikeOut) {
		result |= Font::Style::strikeout;
	}
	if (info.lfUnderline) {
		result |= Font::Style::underline;
	}
	return result;
}


Font Font::ansiFixed()     { return Font(static_cast<HFONT>(GetStockObject(ANSI_FIXED_FONT    ))); }
Font Font::ansiVar()       { return Font(static_cast<HFONT>(GetStockObject(ANSI_VAR_FONT      ))); }
Font Font::deviceDefault() { return Font(static_cast<HFONT>(GetStockObject(DEVICE_DEFAULT_FONT))); }
Font Font::defaultGui()    { return Font(static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT   ))); }
Font Font::oemFixed()      { return Font(static_cast<HFONT>(GetStockObject(OEM_FIXED_FONT     ))); }
Font Font::system()        { return Font(static_cast<HFONT>(GetStockObject(SYSTEM_FONT        ))); }
Font Font::systemFixed()   { return Font(static_cast<HFONT>(GetStockObject(SYSTEM_FIXED_FONT  ))); }



	}
}