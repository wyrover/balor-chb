#include "Graphics.hpp"

#include <algorithm>
#include <memory>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/GraphicsPath.hpp>
#include <balor/graphics/Pen.hpp>
#include <balor/graphics/Region.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/Size.hpp>
#include <balor/String.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

#pragma comment(lib, "msimg32.lib")


namespace balor {
	namespace graphics {


using std::max;
using std::min;
using std::move;
using std::swap;
using std::unique_ptr;


namespace {
static_assert(Graphics::CopyMode::andScans    == STRETCH_ANDSCANS, "Invalid enum value");
static_assert(Graphics::CopyMode::orScans     == STRETCH_ORSCANS, "Invalid enum value");
static_assert(Graphics::CopyMode::deleteScans == STRETCH_DELETESCANS, "Invalid enum value");
static_assert(Graphics::CopyMode::halftone    == STRETCH_HALFTONE, "Invalid enum value");

//static_assert(Graphics::CopyOperation::blackness         == BLACKNESS, "Invalid enum value");
static_assert(Graphics::CopyOperation::captureBlt        == CAPTUREBLT, "Invalid enum value");
static_assert(Graphics::CopyOperation::destinationInvert == DSTINVERT, "Invalid enum value");
static_assert(Graphics::CopyOperation::mergeCopy         == MERGECOPY, "Invalid enum value");
static_assert(Graphics::CopyOperation::mergePaint        == MERGEPAINT, "Invalid enum value");
//static_assert(Graphics::CopyOperation::noMirrorBitmap    == NOMIRRORBITMAP, "Invalid enum value");
static_assert(Graphics::CopyOperation::notSourceCopy     == NOTSRCCOPY, "Invalid enum value");
static_assert(Graphics::CopyOperation::notSourceErase    == NOTSRCERASE, "Invalid enum value");
static_assert(Graphics::CopyOperation::patCopy           == PATCOPY, "Invalid enum value");
static_assert(Graphics::CopyOperation::patInvert         == PATINVERT, "Invalid enum value");
static_assert(Graphics::CopyOperation::patPaint          == PATPAINT, "Invalid enum value");
static_assert(Graphics::CopyOperation::sourceAnd         == SRCAND, "Invalid enum value");
static_assert(Graphics::CopyOperation::sourceCopy        == SRCCOPY, "Invalid enum value");
static_assert(Graphics::CopyOperation::sourceErase       == SRCERASE, "Invalid enum value");
static_assert(Graphics::CopyOperation::sourceInvert      == SRCINVERT, "Invalid enum value");
static_assert(Graphics::CopyOperation::sourcePaint       == SRCPAINT, "Invalid enum value");
//static_assert(Graphics::CopyOperation::whiteness         == WHITENESS, "Invalid enum value");

static_assert(Graphics::TextFormat::bottom                    == (DT_BOTTOM | DT_SINGLELINE), "Invalid enum value");
static_assert(Graphics::TextFormat::default                   == (DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK), "Invalid enum value");
static_assert(Graphics::TextFormat::editControl               == DT_EDITCONTROL, "Invalid enum value");
static_assert(Graphics::TextFormat::endEllipsis               == DT_END_ELLIPSIS, "Invalid enum value");
static_assert(Graphics::TextFormat::expandTabs                == DT_EXPANDTABS, "Invalid enum value");
static_assert(Graphics::TextFormat::externalLeading           == DT_EXTERNALLEADING, "Invalid enum value");
static_assert(Graphics::TextFormat::hidePrefix                == DT_HIDEPREFIX, "Invalid enum value");
static_assert(Graphics::TextFormat::horizontalCenter          == DT_CENTER, "Invalid enum value");
//static_assert(Graphics::TextFormat::internal                  == DT_INTERNAL, "Invalid enum value");
//static_assert(Graphics::TextFormat::left                      == DT_LEFT, "Invalid enum value");
static_assert(Graphics::TextFormat::noClipping                == DT_NOCLIP, "Invalid enum value");
static_assert(Graphics::TextFormat::noFullWidthCharacterBreak == DT_NOFULLWIDTHCHARBREAK, "Invalid enum value");
static_assert(Graphics::TextFormat::noPrefix                  == DT_NOPREFIX, "Invalid enum value");
static_assert(Graphics::TextFormat::pathEllipsis              == DT_PATH_ELLIPSIS, "Invalid enum value");
static_assert(Graphics::TextFormat::prefixOnly                == DT_PREFIXONLY, "Invalid enum value");
static_assert(Graphics::TextFormat::right                     == DT_RIGHT, "Invalid enum value");
static_assert(Graphics::TextFormat::rightToLeft               == DT_RTLREADING, "Invalid enum value");
static_assert(Graphics::TextFormat::singleLine                == DT_SINGLELINE, "Invalid enum value");
//static_assert(Graphics::TextFormat::top                       == DT_TOP, "Invalid enum value");
static_assert(Graphics::TextFormat::verticalCenter            == (DT_VCENTER | DT_SINGLELINE), "Invalid enum value");
static_assert(Graphics::TextFormat::wordBreak                 == DT_WORDBREAK, "Invalid enum value");
static_assert(Graphics::TextFormat::wordEllipsis              == DT_WORD_ELLIPSIS, "Invalid enum value");


inline double angleToRadian(double angle) {
	const double pi = 3.14159265358979323846;
	return angle * pi / 180;
} 


// 弧を描画するGDI関数向けに、開始角度と弧の角度幅から、開始点と終了点を求める
inline void calculateArcStartAndEnd(const Rectangle& rectangle, double startAngle, double sweepAngle, Point& start, Point& end) {
	const double radiusX = static_cast<double>(rectangle.width ) / 2;
	const double radiusY = static_cast<double>(rectangle.height) / 2;
	const double x0 = rectangle.x + rectangle.width  / 2.f + 0.5f;
	const double y0 = rectangle.y + rectangle.height / 2.f + 0.5f;
	const double startRadian = angleToRadian(startAngle             );
	const double endRadian   = angleToRadian(startAngle + sweepAngle);
	start.x = static_cast<int>(x0 + cos(startRadian) * radiusX);
	start.y = static_cast<int>(y0 - sin(startRadian) * radiusY);
	end.x   = static_cast<int>(x0 + cos(endRadian  ) * radiusX);
	end.y   = static_cast<int>(y0 - sin(endRadian  ) * radiusY);
}
} // namespace



bool Graphics::CopyMode::_validate(CopyMode value) {
	return andScans <= value && value <= halftone;
}


bool Graphics::CopyOperation::_validate(CopyOperation /*value*/) {
//	switch (value) {
////		case blackness         :
//		case captureBlt        :
//		case destinationInvert :
//		case mergeCopy         :
//		case mergePaint        :
////		case noMirrorBitmap    :
//		case notSourceCopy     :
//		case notSourceErase    :
//		case patCopy           :
//		case patInvert         :
//		case patPaint          :
//		case sourceAnd         :
//		case sourceCopy        :
//		case sourceErase       :
//		case sourceInvert      :
//		case sourcePaint       : return true;
////		case whiteness         : return true;
//		default                : return false;
//	}
	return true; // 組み合わせ方がありすぎてわからない
}


bool Graphics::TextFormat::_validate(TextFormat value) {
	return (value & ~(
		  bottom                   
		| default                  
		| editControl           
		| endEllipsis              
		| expandTabs               
		| externalLeading          
		| hidePrefix               
		| horizontalCenter         
//		| internal                 
//		| left                     
		| noClipping               
		| noFullWidthCharacterBreak
		| noPrefix                 
		| pathEllipsis             
		| prefixOnly               
		| right                    
		| rightToLeft              
		| singleLine               
//		| top                      
		| verticalCenter           
		| wordBreak                
		| wordEllipsis             
		)) == 0;
}


Graphics::State::State() : _handle(nullptr), _id(0) {
}


Graphics::State::State(HDC _handle) : _handle(_handle), _id(0) {
	if (_handle) {
		_id = SaveDC(_handle);
		assert("Failed to SaveDC" && _id);
	}
}


HDC Graphics::State::handle() const {
	return _handle;
}


int Graphics::State::id() const {
	return _id;
}


Graphics::Graphics() : _handle(nullptr), _owned(false) {
}


Graphics::Graphics(Graphics&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Graphics::Graphics(HDC handle, bool owned) : _handle(handle), _owned(owned) {
}


Graphics::Graphics(HBITMAP bitmap) : _owned(true) {
	assert("Null bitmap" && bitmap);

	_handle = CreateCompatibleDC(0);
	assert("Failed to CreateCompatibleDC" && _handle);
	verify(SelectObject(_handle, bitmap)); // 失敗するならbitmapハンドルが有効ではないか、既にSelectObjectされている
}


Graphics::Graphics(HWND control) : _owned(true) {
	assert("Null control" && control);
	_handle = GetDC(control);
	assert("Failed to GetDC" && _handle);
}


Graphics::~Graphics() {
	if (_handle && _owned) {
		const HWND hwnd = WindowFromDC(_handle);
		if (hwnd) {
			verify(ReleaseDC(hwnd, _handle));
		} else {
			verify(DeleteDC(_handle));
		}
	}
	//_handle = nullptr;
	//_owned = false;
}


Graphics& Graphics::operator=(Graphics&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


Color Graphics::backColor() const {
	assert("Null Graphics handle" && *this);
	auto color = GetBkColor(*this);
	assert("Failed to GetBkColor" && color != CLR_INVALID);
	return Color::fromCOLORREF(color);
}


Color Graphics::backColor(Color value) {
	assert("Null Graphics handle" && *this);
	auto color = SetBkColor(*this, value.toCOLORREF());
	assert("Failed to SetBkColor" && color != CLR_INVALID);
	return Color::fromCOLORREF(color);
}


bool Graphics::backTransparent() const {
	assert("Null Graphics handle" && *this);
	auto mode = GetBkMode(*this);
	assert("Failed to GetBkMode" && mode);
	return mode == TRANSPARENT;
}


bool Graphics::backTransparent(bool value) const {
	assert("Null Graphics handle" && *this);
	auto mode = SetBkMode(*this, value ? TRANSPARENT : OPAQUE);
	assert("Failed to SetBkMode" && mode);
	return mode == TRANSPARENT;
}


//int Graphics::bitsPerPixel() const {
//	assert("Null Graphics handle" && *this);
//	return GetDeviceCaps(*this, BITSPIXEL); // どんなフォーマットの画像から作成しても画面と同じビット深度が帰る
//}


void Graphics::blend(const Point& dstPos, HDC src, float alpha, bool srcAlphaChannel) {
	auto srcRect = Graphics(src).visibleClipBounds();
	blend(dstPos.x, dstPos.y, src, 0, 0, srcRect.width, srcRect.height, alpha, srcAlphaChannel);
}


void Graphics::blend(int dstX, int dstY, HDC src, float alpha, bool srcAlphaChannel) {
	auto srcRect = Graphics(src).visibleClipBounds();
	blend(dstX, dstY, src, 0, 0, srcRect.width, srcRect.height, alpha, srcAlphaChannel);
}


void Graphics::blend(const Point& dstPos, HDC src, const Point& srcPos, const Size& size, float alpha, bool srcAlphaChannel) {
	blend(dstPos.x, dstPos.y, size.width, size.height, src, srcPos.x, srcPos.y, size.width, size.height, alpha, srcAlphaChannel);
}


void Graphics::blend(int dstX, int dstY, HDC src, int srcX, int srcY, int width, int height, float alpha, bool srcAlphaChannel) {
	blend(dstX, dstY, width, height, src, srcX, srcY, width, height, alpha, srcAlphaChannel);
}


void Graphics::blend(const Rectangle& dstRect, HDC src, float alpha, bool srcAlphaChannel) {
	auto srcRect = Graphics(src).visibleClipBounds();
	blend(dstRect.x, dstRect.y, dstRect.width, dstRect.height, src, 0, 0, srcRect.width, srcRect.height, alpha, srcAlphaChannel);
}


void Graphics::blend(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, float alpha, bool srcAlphaChannel) {
	auto srcRect = Graphics(src).visibleClipBounds();
	blend(dstX, dstY, dstWidth, dstHeight, src, 0, 0, srcRect.width, srcRect.height, alpha, srcAlphaChannel);
}


void Graphics::blend(const Rectangle& dstRect, HDC src, const Rectangle& srcRect, float alpha, bool srcAlphaChannel) {
	blend(dstRect.x, dstRect.y, dstRect.width, dstRect.height, src, srcRect.x, srcRect.y, srcRect.width, srcRect.height, alpha, srcAlphaChannel);
}


void Graphics::blend(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, int srcX, int srcY, int srcWidth, int srcHeight, float alpha, bool srcAlphaChannel) {
	assert("Null Graphics handle" && *this);
	assert("Null src handle" && src);
	assert("Mirror copy not supported" && 0 <= dstWidth ); // AlphaBlend の反転コピーはサポートされない
	assert("Mirror copy not supported" && 0 <= dstHeight); // AlphaBlend の反転コピーはサポートされない
	assert("Mirror copy not supported" && 0 <= srcWidth ); // AlphaBlend の反転コピーはサポートされない
	assert("Mirror copy not supported" && 0 <= srcHeight); // AlphaBlend の反転コピーはサポートされない
	const BYTE byteAlpha = static_cast<BYTE>(max(0, min(255, static_cast<int>(alpha * 255 + 0.5f))));
	BLENDFUNCTION function = {AC_SRC_OVER, 0, byteAlpha, 0};
	if (srcAlphaChannel) {
		function.AlphaFormat = AC_SRC_ALPHA;
	}
	verify(AlphaBlend(*this, dstX, dstY, dstWidth, dstHeight, src, srcX, srcY, srcWidth, srcHeight, function));
}


Brush Graphics::brush() const {
	assert("Null Graphics handle" && *this);
	auto brush = static_cast<HBRUSH>(GetCurrentObject(*this, OBJ_BRUSH));
	assert("Failed to GetCurrentObject" && brush);
	return Brush(brush);
}


Brush Graphics::brush(HBRUSH value) {
	assert("Null Graphics handle" && *this);
	auto brush = static_cast<HBRUSH>(SelectObject(*this, value));
	assert("Failed to SelectObject" && brush);
	return Brush(brush);
}


Brush Graphics::brush(Color value) {
	assert("Null Graphics handle" && *this);
	verify(SetDCBrushColor(*this, value.toCOLORREF()) != CLR_INVALID);
	auto dcBrush = static_cast<HBRUSH>(GetStockObject(DC_BRUSH));
	assert("Failed to GetStockObject" && dcBrush);
	auto brush = static_cast<HBRUSH>(SelectObject(*this, dcBrush));
	assert("Failed to SelectObject" && brush);
	return Brush(brush);
}


Point Graphics::brushOrigin() const {
	assert("Null Graphics handle" && *this);
	POINT point;
	verify(GetBrushOrgEx(*this, &point));
	return Point(point);
}


Point Graphics::brushOrigin(const Point& value) {
	assert("Null Graphics handle" && *this);
	POINT point;
	verify(SetBrushOrgEx(*this, value.x, value.y, &point));
	return Point(point);
}


void Graphics::clear() {
	assert("Null Graphics handle" && *this);
	RECT rect;
	verify(GetClipBox(*this, &rect) != ERROR);
	rect.left   -= 1;
	rect.top    -= 1;
	rect.right  += 1;
	rect.bottom += 1;
	drawRectangle(Rectangle(rect));
}


Region Graphics::clip() const {
	assert("Null Graphics handle" && *this);
	Region region;
	region.makeInfinite();
	int result = GetClipRgn(*this, region);
	assert("Failed to GetClipRgn" && result != -1);
	if (result == 0) {
		region = Region();
	}
	return region;
}


void Graphics::clip(HRGN region) {
	assert("Null Graphics handle" && *this);
	verify(SelectClipRgn(*this, region) != ERROR);
}


void Graphics::combineClip(HRGN region, Region::Operation operation) {
	assert("Null Graphics handle" && *this);
	assert("Invalid Region::Operatoin" && Region::Operation::_validate(operation));
	Region temp;
	if (!region) {
		temp.makeInfinite();
		region = temp;
	}
	verify(ExtSelectClipRgn(*this, region, operation) != ERROR);
}


void Graphics::combineClip(const Rectangle rect, Region::Operation operation) {
	assert("Null Graphics handle" && *this);
	assert("Invalid Region::Operatoin" && Region::Operation::_validate(operation));

	switch (operation) {
		case Region::Operation::and : {
			verify(IntersectClipRect(*this, rect.left(), rect.top(), rect.right(), rect.bottom()) != ERROR);
		} break;
		case Region::Operation::diff : {
			verify(ExcludeClipRect(*this, rect.left(), rect.top(), rect.right(), rect.bottom()) != ERROR);
		} break;
		default : {
			combineClip(Region(rect), operation);
		} break;
	}
}


void Graphics::copy(const Point& dstPos, HDC src, Graphics::CopyOperation operation) {
	auto srcRect = Graphics(src).visibleClipBounds();
	copy(dstPos.x, dstPos.y, src, 0, 0, srcRect.width, srcRect.height, operation);
}


void Graphics::copy(int dstX, int dstY, HDC src, Graphics::CopyOperation operation) {
	auto srcRect = Graphics(src).visibleClipBounds();
	copy(dstX, dstY, src, 0, 0, srcRect.width, srcRect.height, operation);
}


void Graphics::copy(const Point& dstPos, HDC src, const Point& srcPos, const Size& size, Graphics::CopyOperation operation) {
	copy(dstPos.x, dstPos.y, src, srcPos.x, srcPos.y, size.width, size.height, operation);
}


void Graphics::copy(int dstX, int dstY, HDC src, int srcX, int srcY, int width, int height, Graphics::CopyOperation operation) {
	assert("Null Graphics handle" && *this);
	assert("Null src handle" && src);
	assert("Invalid Graphics::CopyOperation" && CopyOperation::_validate(operation));

	if (0 <= width && 0 <= height) {
		verify(BitBlt(*this, dstX, dstY, width, height, src, srcX, srcY, operation));
	} else {
		verify(StretchBlt(*this, 0 <= width  ? dstX : dstX - width  - 1
								 , 0 <= height ? dstY : dstY - height - 1
								 , width, height, src, srcX, srcY, abs(width), abs(height), operation));
	}
}


void Graphics::copy(const Rectangle& dstRect, HDC src, const Graphics::CopyOperation operation) {
	auto srcRect = Graphics(src).visibleClipBounds();
	copy(dstRect.x, dstRect.y, dstRect.width, dstRect.height, src, 0, 0, srcRect.width, srcRect.height, operation);
}


void Graphics::copy(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, Graphics::CopyOperation operation) {
	auto srcRect = Graphics(src).visibleClipBounds();
	copy(dstX, dstY, dstWidth, dstHeight, src, 0, 0, srcRect.width, srcRect.height, operation);
}


void Graphics::copy(const Rectangle& dstRect, HDC src, const Rectangle& srcRect, Graphics::CopyOperation operation) {
	copy(dstRect.x, dstRect.y, dstRect.width, dstRect.height, src, srcRect.x, srcRect.y, srcRect.width, srcRect.height, operation);
}


void Graphics::copy(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, int srcX, int srcY, int srcWidth, int srcHeight, Graphics::CopyOperation operation) {
	assert("Null Graphics handle" && *this);
	assert("Null src handle" && src);
	assert("Invalid Graphics::CopyOperation" && CopyOperation::_validate(operation));

	if (dstWidth  < 0 && srcWidth  < 0) { // この処理をしないと反転打消しの場合に表示位置がずれる
		dstWidth  = -dstWidth;
		srcWidth  = -srcWidth;
	}
	if (dstHeight < 0 && srcHeight < 0) {
		dstHeight = -dstHeight;
		srcHeight = -srcHeight;
	}
	verify(StretchBlt(*this, 0 <= dstWidth  ? dstX : dstX - dstWidth  - 1
							 , 0 <= dstHeight ? dstY : dstY - dstHeight - 1
							 , dstWidth, dstHeight, src
							 , 0 <= srcWidth  ? srcX : srcX - srcWidth  - 1
							 , 0 <= srcHeight ? srcY : srcY - srcHeight - 1
							 , srcWidth, srcHeight, operation));
}


Graphics::CopyMode Graphics::copyMode() const {
	assert("Null Graphics handle" && *this);
	auto mode = GetStretchBltMode(*this);
	assert("Failed to GetStretchBltMode" && mode);
	return static_cast<CopyMode>(mode);
}


Graphics::CopyMode Graphics::copyMode(Graphics::CopyMode value) {
	assert("Null Graphics handle" && *this);
	assert("Invalid Graphics::CopyMode" && CopyMode::_validate(value));
	auto mode = SetStretchBltMode(*this, value);
	assert("Failed to SetStretchBltMode" && mode);
	return static_cast<CopyMode>(mode);
}


int Graphics::dpiX() const {
	assert("Null Graphics handle" && *this);
	return GetDeviceCaps(*this, LOGPIXELSX);
}


int Graphics::dpiY() const {
	assert("Null Graphics handle" && *this);
	return GetDeviceCaps(*this, LOGPIXELSY);
}


void Graphics::drawArc(const Rectangle& rect, float startAngle, float sweepAngle) {
	assert("Null Graphics handle" && *this);

	if (rect.width <= 0 || rect.height <= 0) { // 半径０なのに点が残るのを防ぐ
		return;
	}
	const float absSweepAngle = abs(sweepAngle);
	if (absSweepAngle < FLT_EPSILON) { // 描画角度０なのに点が残るのを防ぐ
		return;
	}
	if (360.f - FLT_EPSILON < absSweepAngle) { // 円を描こうとすると開始点と終了点が重なってしまう
		float delta = 0.f <= sweepAngle ? 180.f : -180.f;
		drawArc(rect, startAngle, delta); // 半円ずつ描く
		drawArc(rect, startAngle + delta, delta);
	} else {
		Point start, end;
		if (sweepAngle < 0.f) {
			startAngle += sweepAngle;
			sweepAngle = -sweepAngle;
		}
		calculateArcStartAndEnd(rect, startAngle, sweepAngle, start, end);
		verify(Arc(*this, rect.x, rect.y, rect.right(), rect.bottom(), start.x, start.y, end.x, end.y));
	}
}


void Graphics::drawArc(int x, int y, int width, int height, float startAngle, float sweepAngle) {
	drawArc(Rectangle(x, y, width, height), startAngle, sweepAngle);
}


void Graphics::drawBezier(const Point& p0, const Point& p1, const Point& p2, const Point& p3) {
	const Point points[] = {p0, p1, p2, p3};
	drawBeziers(points);
}


void Graphics::drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	const Point points[] = {Point(x0, y0), Point(x1, y1), Point(x2, y2), Point(x3, y3)};
	drawBeziers(points);
}


void Graphics::drawBeziers(ArrayRange<const Point> points) {
	assert("Null Graphics handle" && *this);
	assert("Invalid points size" && 4 <= points.size() && (points.size() % 3) == 1);
	verify(PolyBezier(*this, reinterpret_cast<const POINT*>(points.begin()), static_cast<DWORD>(points.size())));
}


void Graphics::drawCircle(const Point& point, int radius) {
	drawEllipse(Rectangle(point.x - radius, point.y - radius, radius * 2, radius * 2));
}


void Graphics::drawCircle(int x, int y, int radius) {
	drawEllipse(Rectangle(x - radius, y - radius, radius * 2, radius * 2));
}


void Graphics::drawEllipse(const Rectangle& rect) {
	assert("Null Graphics handle" && *this);
	if (0 < rect.width && 0 < rect.height) {
		verify(Ellipse(*this, rect.left(), rect.top(), rect.right(), rect.bottom()));
	}
}


void Graphics::drawEllipse(int x, int y, int width, int height) {
	drawEllipse(Rectangle(x, y, width, height));
}


void Graphics::drawIcon(HICON icon, const Point& point, bool masked) {
	drawIcon(icon, point.x, point.y, 0, 0, masked);
}


void Graphics::drawIcon(HICON icon, int x, int y, bool masked) {
	drawIcon(icon, x, y, 0, 0, masked);
}


void Graphics::drawIcon(HICON icon, const Rectangle& rect, bool masked) {
	drawIcon(icon, rect.x, rect.y, rect.width, rect.height, masked);
}


void Graphics::drawIcon(HICON icon, int x, int y, int width, int height, bool masked) {
	assert("Null Graphics handle" && *this);
	assert("Null icon" && icon);
	verify(DrawIconEx(*this, x, y, icon, width, height, 0, nullptr, masked ? DI_NORMAL : DI_IMAGE));
}


void Graphics::drawImageList(HIMAGELIST imageList, int index, const Rectangle& rect, ImageList::State state, float alpha, bool grayscale) {
	drawImageList(imageList, index, rect.x, rect.y, rect.width, rect.height, state, alpha, grayscale);
}


void Graphics::drawImageList(HIMAGELIST imageList, int index, int x, int y, int width, int height, ImageList::State state, float alpha, bool grayscale) {
	assert("Null Graphics handle" && *this);
	assert("Null imageList" && imageList);
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < ImageList_GetImageCount(imageList));
	assert("Invalid ImageList::State" && ImageList::State::_validate(state));
	IMAGELISTDRAWPARAMS params;
	params.cbSize = sizeof(params);
	params.himl = imageList;
	params.i = index;
	params.hdcDst = *this;
	params.x = x;
	params.y = y;
	params.cx = width;
	params.cy = height;
	params.xBitmap = 0;
	params.yBitmap = 0;
	params.rgbBk = CLR_NONE;
	params.rgbFg = CLR_DEFAULT;
	params.fStyle = state | ((width || height) ? ILD_SCALE : 0);
	params.dwRop = SRCCOPY;
	params.Frame = max(0, min(255, static_cast<int>(alpha * 255.f + 0.5f)));
	params.fState = grayscale ? ILS_SATURATE : (params.Frame < 255 ? ILS_ALPHA : ILS_NORMAL);
	params.crEffect = 0;
	verify(ImageList_DrawIndirect(&params));
}


void Graphics::drawLine(const Point& p0, const Point& p1) {
	Point points[] = {p0, p1};
	drawLines(points);
}


void Graphics::drawLine(int x0, int y0, int x1, int y1) {
	Point points[] = {Point(x0, y0), Point(x1, y1)};
	drawLines(points);
}


void Graphics::drawLines(ArrayRange<const Point> points) {
	assert("Null Graphics handle" && *this);
	assert("Invalid points size" && 2 <= points.size());
	verify(Polyline(*this, reinterpret_cast<const POINT*>(points.begin()), static_cast<DWORD>(points.size())));
}


void Graphics::drawPath(GraphicsPath& path) {
	assert("Null Graphics handle" && *this);
	GraphicsPath::Data data = path.data();
	if (0 < data.points.size()) {
		verify(PolyDraw(*this, reinterpret_cast<const POINT*>(data.points.data()), data.types.data(), data.points.size()));
	}
}

	
void Graphics::drawPie(const Rectangle& rect, float startAngle, float sweepAngle) {
	assert("Null Graphics handle" && *this);

	if (rect.width <= 0 || rect.height <= 0) {
		return;
	}
	const float absSweepAngle = abs(sweepAngle);
	if (absSweepAngle < FLT_EPSILON) {
		return;
	}
	if (360.f - FLT_EPSILON < absSweepAngle) {
		drawEllipse(rect);
	} else {
		Point start, end;
		if (sweepAngle < 0.f) {
			startAngle += sweepAngle;
			sweepAngle = -sweepAngle;
		}
		calculateArcStartAndEnd(rect, startAngle, sweepAngle, start, end);
		verify(Pie(*this, rect.left(), rect.top(), rect.right(), rect.bottom(), start.x, start.y, end.x, end.y));
	}
}


void Graphics::drawPie(int x, int y, int width, int height, float startAngle, float sweepAngle) {
	drawPie(Rectangle(x, y, width, height), startAngle, sweepAngle);
}


void Graphics::drawPolygon(ArrayRange<const Point> points) {
	assert("Null Graphics handle" && *this);
	assert("Invalid points size" && 2 <= points.size());
	verify(Polygon(*this, reinterpret_cast<const POINT*>(points.begin()), static_cast<DWORD>(points.size())));
}


void Graphics::drawRectangle(const Rectangle& rect) {
	assert("Null Graphics handle" && *this);
	if (0 < rect.width && 0 < rect.height) {
		verify(::Rectangle(*this, rect.left(), rect.top(), rect.right(), rect.bottom()));
	}
}


void Graphics::drawRectangle(int x, int y, int width, int height) {
	drawRectangle(Rectangle(x, y, width, height));
}


void Graphics::drawRectangles(ArrayRange<const Rectangle> rects) {
	assert("Null Graphics handle" && *this);
	for (auto i = rects.begin(), end = rects.end(); i != end; ++i) {
		if (0 < i->width && 0 < i->height) {
			verify(::Rectangle(*this, i->left(), i->top(), i->right(), i->bottom()));
		}
	}
}


void Graphics::drawRegion(HRGN region, HBRUSH brush, int lineWidth, int lineHeight) {
	assert("Null Graphics handle" && *this);
	assert("Null region" && region);
	assert("Null brush" && brush);
	verify(FrameRgn(*this, region, brush, lineWidth, lineHeight));
}


void Graphics::drawRoundRectangle(const Rectangle& rect, const Size ellipseSize) {
	assert("Null Graphics handle" && *this);
	if (0 < rect.width && 0 < rect.height) {
		verify(::RoundRect(*this, rect.left(), rect.top(), rect.right(), rect.bottom(), ellipseSize.width, ellipseSize.height));
	}
}


void Graphics::drawRoundRectangle(int x, int y, int width, int height, int ellipseWidth, int ellipseHeight) {
	drawRoundRectangle(Rectangle(x, y, width, height), Size(ellipseWidth, ellipseHeight));
}


void Graphics::drawText(StringRange s, const Point& point) {
	drawText(s, point.x, point.y);
}


void Graphics::drawText(StringRange s, int x, int y) {
	assert("Null Graphics handle" && *this);
	verify(TextOutW(*this, x, y, s.c_str(), static_cast<int>(s.length())));
}


void Graphics::drawText(StringRange s, const Rectangle& rect, Graphics::TextFormat format, int tabWidth) {
	assert("Null Graphics handle" && *this);
	assert("Invalid Graphics::TextFormat" && TextFormat::_validate(format));
	DRAWTEXTPARAMS params;
	params.cbSize = sizeof(params);
	params.iTabLength = tabWidth;
	params.iLeftMargin = 0;
	params.iRightMargin = 0;
	RECT winRect = rect;
	verify(DrawTextExW(*this, const_cast<wchar_t*>(s.c_str()), static_cast<int>(s.length()), &winRect, format | DT_TABSTOP, &params));
}


void Graphics::drawText(StringRange s, int x, int y, int width, int height, Graphics::TextFormat format, int tabWidth) {
	drawText(s, Rectangle(x, y, width, height), format, tabWidth);
}


bool Graphics::fillAlternate() const {
	assert("Null Graphics handle" && *this);
	auto mode = GetPolyFillMode(*this);
	assert("Failed to GetPolyFillMode" && mode);
	return mode == ALTERNATE;
}


bool Graphics::fillAlternate(bool value) {
	assert("Null Graphics handle" && *this);
	auto mode = SetPolyFillMode(*this, value ? ALTERNATE : WINDING);
	assert("Failed to SetPolyFillMode" && mode);
	return mode == ALTERNATE;
}


void Graphics::fillPath(GraphicsPath& path) {
	assert("Null Graphics handle" && *this);
	const GraphicsPath::Data data = path.data();
	if (0 < data.points.size()) {
		verify(BeginPath(*this));
		verify(PolyDraw(*this, reinterpret_cast<const POINT*>(data.points.data()), data.types.data(), data.points.size()));
		verify(EndPath(*this));
		verify(FillPath(*this));
	}
}


void Graphics::fillRegion(HRGN region) {
	assert("Null Graphics handle" && *this);
	if (region) {
		verify(PaintRgn(*this, region));
	} else {
		clear();
	}
}


void Graphics::floodFillBorder(const Point& point, const Color& borderColor) {
	floodFillBorder(point.x, point.y, borderColor);
}


void Graphics::floodFillBorder(int x, int y, const Color& borderColor) {
	assert("Null Graphics handle" && *this);
	ExtFloodFill(*this, x, y, borderColor.toCOLORREF(), FLOODFILLBORDER);
}


void Graphics::floodFillSurface(const Point& point, const Color& surfaceColor) {
	floodFillSurface(point.x, point.y, surfaceColor);
}


void Graphics::floodFillSurface(int x, int y, const Color& surfaceColor) {
	assert("Null Graphics handle" && *this);
	ExtFloodFill(*this, x, y, surfaceColor.toCOLORREF(), FLOODFILLSURFACE);
}


void Graphics::flush() {
	verify(GdiFlush());
}


Font Graphics::font() const {
	assert("Null Graphics handle" && *this);
	auto font = static_cast<HFONT>(GetCurrentObject(*this, OBJ_FONT));
	assert("Failed to GetCurrentObject" && font);
	return Font(font);
}


Font Graphics::font(HFONT value) {
	assert("Null Graphics handle" && *this);
	auto font = static_cast<HFONT>(SelectObject(*this, value));
	assert("Failed to SelectObject" && font);
	return Font(font);
}


Graphics Graphics::fromControlWithNonClientArea(HWND control) {
	assert("Null control" && control);
	Graphics graphics(GetWindowDC(control), true);
	assert("Failed to GetWindowDC" && graphics);
	return graphics;
}


Graphics Graphics::fromGraphics(HDC graphics) {
	Graphics compatibleGraphics(CreateCompatibleDC(graphics), true);
	assert("Failed to CreateCompatibleDC from screen" && compatibleGraphics);
	return compatibleGraphics;
}


Graphics Graphics::fromScreen() {
	Graphics graphics(GetDC(nullptr), true);
	assert("Failed to GetDC from screen" && graphics);
	return graphics;
}


Color Graphics::getNearestColor(const Color& color) const {
	assert("Null Graphics handle" && *this);
	const COLORREF result = GetNearestColor(*this, color.toCOLORREF());
	assert(result != CLR_INVALID);
	return Color::fromCOLORREF(result);
}


Color Graphics::getPixel(const Point& point) const {
	return getPixel(point.x, point.y);
}


Color Graphics::getPixel(int x, int y) const {
	assert("Null Graphics handle" && *this);
	const COLORREF color = GetPixel(*this, x, y);
	return Color::fromCOLORREF(color);
}


bool Graphics::isVisible(const Point& point) const {
	return isVisible(point.x, point.y);
}


bool Graphics::isVisible(int x, int y) const {
	assert("Null Graphics handle" && *this);
	return PtVisible(*this, x, y) != 0;
}


bool Graphics::isVisible(const Rectangle& rect) const {
	assert("Null Graphics handle" && *this);
	RECT temp = rect;
	return RectVisible(*this, &temp) != 0;
}


bool Graphics::isVisible(int x, int y, int width, int height) const {
	return isVisible(Rectangle(x, y, width, height));
}


Size Graphics::measureText(StringRange s) {
	assert("Null Graphics handle" && *this);
	if (s.length() == 0) {
		return Size(0, 0);
	}
	SIZE size;
	verify(GetTextExtentPoint32W(*this, s.c_str(), s.length(), &size));
	return Size(size.cx, size.cy);
}


Size Graphics::measureText(StringRange s, int width) {
	assert("Null Graphics handle" && *this);
	assert("Negative width" && 0 <= width);

	if (s.length() == 0) {
		return Size(0, 0);
	}
	SIZE size;
	int fit = s.length();
	unique_ptr<int[]> extents(new int[s.length()]);
	verify(GetTextExtentExPointW(*this, s.c_str(), s.length(), width, &fit, extents.get(), &size));
	assert(0 <= fit);
	assert(fit <= s.length());
	return Size(0 < fit ? extents[fit - 1] : 0, size.cy);
}


Size Graphics::measureText(StringRange s, const Size& layoutArea, Graphics::TextFormat format, int tabWidth) {
	return measureText(s, layoutArea.width, layoutArea.height, format, tabWidth);
}


Size Graphics::measureText(StringRange s, int width, int height, Graphics::TextFormat format, int tabWidth) {
	assert("Null Graphics handle" && *this);
	assert("Invalid Graphics::TextFormat" && TextFormat::_validate(format));

	if (s.length() == 0) {
		return Size(0, 0);
	}
	RECT rect = {0, 0, width, height};
	DRAWTEXTPARAMS params;
	params.cbSize = sizeof(params);
	params.iTabLength = tabWidth;
	params.iLeftMargin = 0;
	params.iRightMargin = 0;
	verify(DrawTextExW(*this, const_cast<wchar_t*>(s.c_str()), static_cast<int>(s.length()), &rect, format | DT_CALCRECT | DT_TABSTOP, &params));
	return Size(min(static_cast<int>(rect.right ), width )
			  , min(static_cast<int>(rect.bottom), height));
}


float Graphics::miterLimit() const {
	assert("Null Graphics handle" && *this);
	float result;
	verify(GetMiterLimit(*this, &result));
	return result;
}


float Graphics::miterLimit(float value) {
	assert("Null Graphics handle" && *this);
	float old;
	verify(SetMiterLimit(*this, value, &old));
	return old;
}


bool Graphics::owned() const {
	return _owned;
}


void Graphics::owned(bool value) {
	_owned = value;
}


Pen Graphics::pen() const {
	assert("Null Graphics handle" && *this);
	auto pen = static_cast<HPEN>(GetCurrentObject(*this, OBJ_PEN));
	assert("Failed to GetCurrentObject" && pen);
	return Pen(pen);
}


Pen Graphics::pen(HPEN value) {
	assert("Null Graphics handle" && *this);
	auto pen = static_cast<HPEN>(SelectObject(*this, value));
	assert("Failed to SelectObject" && pen);
	return Pen(pen);
}


Pen Graphics::pen(Color value) {
	assert("Null Graphics handle" && *this);
	verify(SetDCPenColor(*this, value.toCOLORREF()) != CLR_INVALID);
	auto dcPen = static_cast<HPEN>(GetStockObject(DC_PEN));
	assert("Failed to GetStockObject" && dcPen);
	auto pen = static_cast<HPEN>(SelectObject(*this, dcPen));
	assert("Failed to SelectObject" && pen);
	return Pen(pen);
}


int Graphics::pointToPixel(float point) {
	assert("Null Graphics handle" && *this);
	const int logicalPixelsY = GetDeviceCaps(*this, LOGPIXELSY);
	return static_cast<int>((point * logicalPixelsY / 72) + 0.5f);
	//return MulDiv(static_cast<int>(point), logicalPixelsY, 72);
}


float Graphics::pixelToPoint(int pixel) {
	assert("Null Graphics handle" && *this);
	const int logicalPixelsY = GetDeviceCaps(*this, LOGPIXELSY);
	return ((pixel * 72) / static_cast<float>(logicalPixelsY)) + 0.5f;
	//return static_cast<float>(MulDiv(pixel, 72, logicalPixelsY));
}


void Graphics::setPixel(const Point& point, const Color& color) {
	setPixel(point.x, point.y, color);
}


void Graphics::setPixel(int x, int y, const Color& color) {
	assert("Null Graphics handle" && *this);
	verify(SetPixel(*this, x, y, color.toCOLORREF()) != -1);
}


Graphics::State Graphics::state() const {
	assert("Null Graphics handle" && *this);
	return State(*this);
}


void Graphics::state(Graphics::State value) {
	assert("Null Graphics handle" && *this);
	assert("Invalid Graphics::State" && value.handle() == *this);
	verify(RestoreDC(*this, value.id()));
}


int Graphics::textCharExtra() const {
	assert("Null Graphics handle" && *this);

	int result = GetTextCharacterExtra(*this);
	assert(result != 0x8000000);
	return result;
}


int Graphics::textCharExtra(int value) {
	assert("Null Graphics handle" && *this);
	int old = SetTextCharacterExtra(*this, value);
	assert("Failed to SetTextCharacterExtra" && old != 0x8000000);
	return old;
}


Color Graphics::textColor() const {
	assert("Null Graphics handle" && *this);
	auto color = GetTextColor(*this);
	assert("Failed to GetTextColor" && color != CLR_INVALID);
	return Color::fromCOLORREF(color);
}


Color Graphics::textColor(Color value) {
	assert("Null Graphics handle" && *this);
	auto color = SetTextColor(*this, value.toCOLORREF());
	assert("Failed to SetTextColor" && color != CLR_INVALID);
	return Color::fromCOLORREF(color);
}


void Graphics::translateClip(int dx, int dy) {
	assert("Null Graphics handle" && *this);
	verify(OffsetClipRgn(*this, dx, dy) != ERROR);
}


Rectangle Graphics::visibleClipBounds() const {
	assert("Null Graphics handle" && *this);
	RECT rect;
	verify(GetClipBox(*this, &rect) != ERROR);
	return Rectangle(rect);
}



	}
}