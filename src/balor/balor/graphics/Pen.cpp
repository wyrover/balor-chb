#include "Pen.hpp"

#include <vector>

#include <balor/graphics/Color.hpp>
#include <balor/graphics/Brush.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>


namespace balor {
	namespace graphics {


using std::move;
using std::swap;
using std::vector;


namespace {
static_assert(Pen::Style::solid       == PS_SOLID, "Invalid enum value");
static_assert(Pen::Style::dash        == PS_DASH, "Invalid enum value");
static_assert(Pen::Style::dot         == PS_DOT, "Invalid enum value");
static_assert(Pen::Style::dashDot     == PS_DASHDOT, "Invalid enum value");
static_assert(Pen::Style::dashDotDot  == PS_DASHDOTDOT, "Invalid enum value");
static_assert(Pen::Style::custom      == PS_USERSTYLE, "Invalid enum value");
static_assert(Pen::Style::null        == PS_NULL, "Invalid enum value");
static_assert(Pen::Style::insideFrame == PS_INSIDEFRAME, "Invalid enum value");

static_assert(Pen::LineCap::round  == PS_ENDCAP_ROUND, "Invalid enum value");
static_assert(Pen::LineCap::square == PS_ENDCAP_SQUARE, "Invalid enum value");
static_assert(Pen::LineCap::flat   == PS_ENDCAP_FLAT, "Invalid enum value");

static_assert(Pen::LineJoin::round  == PS_JOIN_ROUND, "Invalid enum value");
static_assert(Pen::LineJoin::bevel  == PS_JOIN_BEVEL, "Invalid enum value");
static_assert(Pen::LineJoin::mitter == PS_JOIN_MITER, "Invalid enum value");


HPEN createPen(const Color* color, HBRUSH brush, int width, Pen::Style penStyle, Pen::LineCap lineCap, Pen::LineJoin lineJoin, ArrayRange<const int>* patten) {
	DWORD style = 0;
	if (width == 0) {
		style = PS_COSMETIC | PS_NULL;
		brush = nullptr;
		patten = nullptr;
	} else if (brush || 1 < width || patten) {
		style = penStyle | lineCap | lineJoin | PS_GEOMETRIC;
	} else {
		style = PS_COSMETIC | penStyle;
	}

	LOGBRUSH logbrush;
	if (brush) {
		verify(GetObjectW(brush, sizeof(logbrush), &logbrush)); // 失敗するなら brush が有効なブラシのハンドルではない
	} else {
		logbrush.lbStyle = BS_SOLID;
		logbrush.lbColor = color->toCOLORREF();
		logbrush.lbHatch = 0;
	}
	HPEN handle = ExtCreatePen(style, width, &logbrush, patten ? patten->length() : 0
							 , reinterpret_cast<const DWORD*>(patten ? patten->begin() : nullptr));
	assert("Failed to ExtCreatePen" && handle); // ペンの作成に失敗した。テクスチャブラシのビットマップが既に破棄されてないか？
	return handle;
}


struct PenInfo {
	PenInfo(HPEN handle) {
		assert("Null Pen handle" && handle);
		int size = GetObjectW(handle, 0, nullptr);
		if (size <= sizeof(EXTLOGPEN)) {
			verify(GetObjectW(handle, sizeof(EXTLOGPEN), &_logPen));
			_fullLogPen = &_logPen;
		} else { // パターン情報が末尾に追加されている
			_fullLogPen = reinterpret_cast<EXTLOGPEN*>(new BYTE[size]);
			verify(GetObjectW(handle, size, _fullLogPen));
		}
	}
	~PenInfo() {
		if (_fullLogPen != &_logPen) {
			delete [] reinterpret_cast<BYTE*>(_fullLogPen);
		}
	}

	EXTLOGPEN* operator->() {
		return _fullLogPen;
	}

	EXTLOGPEN _logPen;
	EXTLOGPEN* _fullLogPen;
};
} // namespace



bool Pen::LineCap::_validate(LineCap value) {
	switch (value) {
		case round  :
		case square :
		case flat   : return true;
		default     : return false;
	}
}


bool Pen::LineJoin::_validate(LineJoin value) {
	switch (value) {
		case round  :
		case bevel  :
		case mitter : return true;
		default     : return false;
	}
}


bool Pen::Style::_validate(Style value) {
	return solid <= value && value <= custom;
}



Pen::Pen() : _handle(nullptr), _owned(false) {
}


Pen::Pen(HPEN handle, bool owned) : _handle(handle), _owned(owned) {
}


Pen::Pen(Pen&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Pen::Pen(HBRUSH brush, int width, Pen::Style style, Pen::LineCap lineCap, Pen::LineJoin lineJoin) : _owned(true) {
	assert("Null brush" && brush);
	assert("Negative width" && 0 <= width);
	assert("Invalid Pen::Style" && Style::_validate(style) && style != Style::custom);
	assert("Invalid Pen::LineCap" && LineCap::_validate(lineCap));
	assert("Invalid Pen::LineJoin" && LineJoin::_validate(lineJoin));
	_handle = createPen(nullptr, brush, width, style, lineCap, lineJoin, nullptr);
}


Pen::Pen(HBRUSH brush, int width, ArrayRange<const int> patten, Pen::LineCap lineCap, Pen::LineJoin lineJoin) : _owned(true) {
	assert("Null brush" && brush);
	assert("Negative width" && 0 <= width);
	assert("Empty patten" && !patten.empty());
	assert("Invalid lineCap" && LineCap::_validate(lineCap));
	assert("Invalid lineJoin" && LineJoin::_validate(lineJoin));
	_handle = createPen(nullptr, brush, width, Pen::Style::custom, lineCap, lineJoin, &patten);
}


Pen::Pen(const Color& color, int width, Pen::Style style, Pen::LineCap lineCap, Pen::LineJoin lineJoin) : _owned(true) {
	assert("Negative width" && 0 <= width);
	assert("Invalid Pen::Style" && Style::_validate(style) && style != Style::custom);
	assert("Invalid Pen::LineCap" && LineCap::_validate(lineCap));
	assert("Invalid Pen::LineJoin" && LineJoin::_validate(lineJoin));
	_handle = createPen(&color, nullptr, width, style, lineCap, lineJoin, nullptr);
}


Pen::Pen(const Color& color, int width, ArrayRange<const int> patten, Pen::LineCap lineCap, Pen::LineJoin lineJoin) : _owned(true) {
	assert("Negative width" && 0 <= width);
	assert("Empty patten" && !patten.empty());
	assert("Invalid Pen::LneCap" && LineCap::_validate(lineCap));
	assert("Invalid Pen::LineJoin" && LineJoin::_validate(lineJoin));
	_handle = createPen(&color, nullptr, width, Pen::Style::custom, lineCap, lineJoin, &patten);
}


Pen::~Pen() {
	if (_handle && _owned) {
		verify(DeleteObject(_handle)); // ペンが SelectObject されたままか、または既に有効なハンドルではない
	}
	//_owned = false;
	//_handle = nullptr;
}


Pen& Pen::operator=(Pen&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


Brush Pen::brush() const {
	PenInfo info(*this);
	switch (info->elpBrushStyle) {
		case BS_NULL          : { // 実際には NULLブラシから作成しても黒のソリッドブラシになるのでここには来ない
			return Brush::hollow();
		}
		case BS_HATCHED       : {
			return Brush(Color::fromCOLORREF(info->elpColor), static_cast<Brush::Hatch>(info->elpHatch));
		}
		case BS_PATTERN       : {
			return Brush(reinterpret_cast<HBITMAP>(info->elpHatch));
		}
	}
	//assert(info->elpBrushStyle == BS_SOLID);
	return Brush();
}


Pen Pen::clone() const {
	if (!*this) {
		return Pen();
	}
	return clone(_handle);
}


Pen Pen::clone(HPEN handle) {
	assert("Null handle" && handle);
	PenInfo info(handle);
	LOGBRUSH logbrush;
	logbrush.lbStyle = info->elpBrushStyle;
	logbrush.lbColor = info->elpColor     ;
	logbrush.lbHatch = info->elpHatch     ;
	Pen pen(ExtCreatePen(info->elpPenStyle, info->elpWidth, &logbrush, info->elpNumEntries, info->elpNumEntries == 0 ? nullptr : info->elpStyleEntry), true);
	assert("Failed to ExtCreatePen" && pen);
	return pen;
}


Color Pen::color() const {
	return Color::fromCOLORREF(PenInfo(*this)->elpColor);
}


Pen::LineCap Pen::lineCap() const {
	return static_cast<LineCap>(PenInfo(*this)->elpPenStyle & PS_ENDCAP_MASK);
}


Pen::LineJoin Pen::lineJoin() const {
	return static_cast<LineJoin>(PenInfo(*this)->elpPenStyle & PS_JOIN_MASK);
}


bool Pen::owned() const {
	return _owned;
}


void Pen::owned(bool value) {
	_owned = value;
}


vector<int> Pen::pattern() const {
	PenInfo info(*this);
	vector<int> result;
	if (0 < info->elpNumEntries) {
		result.assign(info->elpStyleEntry, info->elpStyleEntry + info->elpNumEntries);
	}
	return result;
}


Pen::Style Pen::style() const {
	return static_cast<Pen::Style>(PenInfo(*this)->elpPenStyle & PS_STYLE_MASK);
}


int Pen::width() const {
	return PenInfo(*this)->elpWidth;
}


Pen Pen::white()  { return Pen(static_cast<HPEN>(GetStockObject(WHITE_PEN))); }
Pen Pen::black()  { return Pen(static_cast<HPEN>(GetStockObject(BLACK_PEN))); }
Pen Pen::hollow() { return Pen(static_cast<HPEN>(GetStockObject(NULL_PEN ))); }




	}
}