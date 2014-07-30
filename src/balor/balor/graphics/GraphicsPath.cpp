#include "GraphicsPath.hpp"

#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Region.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/scopeExit.hpp>
#include <balor/Size.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace graphics {

using std::swap;


namespace {
	void getPathData(HDC handle, GraphicsPath::Data& data) {
		const int size = GetPath(handle, nullptr, nullptr, 0);
		assert(size != -1);
		if (0 < size) {
			data.points.resize(size);
			data.types.resize(size);
			verify(GetPath(handle, reinterpret_cast<POINT*>(data.points.data()), data.types.data(), size) == size);
		}
		verify(BeginPath(handle));
		if (0 < size) { // 取得したパスデータの書き戻し
			verify(PolyDraw(handle, reinterpret_cast<const POINT*>(data.points.data()), data.types.data(), size));
		}
	}


	inline double angleToRadian(double angle) {
		const double pi = 3.14159265358979323846;
		return angle * pi / 180;
	}


	// 弧を描画するGDI関数向けに、開始角度と弧の角度幅から、開始点と終了点を求める
	inline void calculateArcStartAndEnd(const Rectangle& rectangle, double startAngle, double sweepAngle, Point& start, Point& end) {
		const double radiusX = static_cast<double>(rectangle.width) / 2;
		const double radiusY = static_cast<double>(rectangle.height) / 2;
		const double x0 = rectangle.x + rectangle.width / 2.f + 0.5f;
		const double y0 = rectangle.y + rectangle.height / 2.f + 0.5f;
		const double startRadian = angleToRadian(startAngle);
		const double endRadian = angleToRadian(startAngle + sweepAngle);
		start.x = static_cast<int>(x0 + cos(startRadian) * radiusX);
		start.y = static_cast<int>(y0 - sin(startRadian) * radiusY);
		end.x = static_cast<int>(x0 + cos(endRadian) * radiusX);
		end.y = static_cast<int>(y0 - sin(endRadian) * radiusY);
	}
} // namespace


GraphicsPath::Data::Data(Data&& value) : points(move(value.points)), types(move(value.types)) {
}


GraphicsPath::Data& GraphicsPath::Data::operator=(Data&& value) {
	swap(points, value.points);
	swap(types, value.types);
	return *this;
}



GraphicsPath::GraphicsPath() : _closed(true) {
	_handle = CreateCompatibleDC(nullptr);
	assert("Failed to CreateCompatibleDC" && _handle);
	verify(BeginPath(_handle));
}


GraphicsPath::GraphicsPath(GraphicsPath&& value) : _handle(value._handle), _closed(value._closed) {
	value._handle = nullptr;
}


GraphicsPath::GraphicsPath(const Data& data) : _handle(nullptr), _closed(true) {
	assert("Invalid data size" && data.points.size() == data.types.size());
	*this = GraphicsPath();
	if (0 < data.points.size()) {
		verify(PolyDraw(_handle, reinterpret_cast<const POINT*>(data.points.data()), data.types.data(), data.points.size()));
	}
}


GraphicsPath::~GraphicsPath() {
	if (_handle) {
		verify(AbortPath(_handle)); // 意味は無いかもしれないが一応
		verify(DeleteDC(_handle));
		//_handle = nullptr;
	}
}


GraphicsPath& GraphicsPath::operator=(GraphicsPath&& value) {
	swap(_handle, value._handle);
	swap(_closed, value._closed);
	return *this;
}


void GraphicsPath::addArc(const Rectangle& rect, float startAngle, float sweepAngle) {
	assert("Null GraphicsPath handle" && _handle);

	if (rect.width <= 0 || rect.height <= 0) {
		return;
	}
	float absSweepAngle = abs(sweepAngle);
	if (absSweepAngle < FLT_EPSILON) {
		return;
	}
	if (360.f - FLT_EPSILON < sweepAngle) {
		sweepAngle = 360.f - FLT_EPSILON;
	}
	if (sweepAngle < -360.f + FLT_EPSILON) {
		sweepAngle = -360.f + FLT_EPSILON;
	}
	Point start, end;
	verify(SetArcDirection(_handle, 0.f < sweepAngle ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE));
	calculateArcStartAndEnd(rect, startAngle, sweepAngle, start, end);
	if (_closed) {
		verify(MoveToEx(_handle, start.x, start.y, nullptr));
	}
	verify(ArcTo(_handle, rect.x, rect.y, rect.right(), rect.bottom(), start.x, start.y, end.x, end.y));
	_closed = false;
}


void GraphicsPath::addArc(int x, int y, int width, int height, float startAngle, float sweepAngle) {
	addArc(Rectangle(x, y, width, height), startAngle, sweepAngle);
}


void GraphicsPath::addBezier(const Point& p0, const Point& p1, const Point& p2, const Point& p3) {
	const Point points[] = { p0, p1, p2, p3 };
	addBeziers(points);
}


void GraphicsPath::addBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	const Point points[] = { Point(x0, y0), Point(x1, y1), Point(x2, y2), Point(x3, y3) };
	addBeziers(points);
}


void GraphicsPath::addBeziers(ArrayRange<const Point> points) {
	assert("Null GraphicsPath handle" && _handle);
	assert("Invalid points size" && 4 <= points.size() && (points.size() % 3) == 1);
	if (_closed) {
		verify(MoveToEx(_handle, points[0].x, points[0].y, nullptr));
	}
	else {
		verify(LineTo(_handle, points[0].x, points[0].y));
	}
	verify(PolyBezierTo(_handle, reinterpret_cast<const POINT*>(points.begin() + 1), static_cast<DWORD>(points.size() - 1)));
	_closed = false;
}


void GraphicsPath::addCircle(const Point& point, int radius) {
	addEllipse(Rectangle(point.x - radius, point.y - radius, radius * 2, radius * 2));
}


void GraphicsPath::addCircle(int x, int y, int radius) {
	addEllipse(Rectangle(x - radius, y - radius, radius * 2, radius * 2));
}


void GraphicsPath::addEllipse(const Rectangle& rect) {
	assert("Null GraphicsPath handle" && _handle);
	if (0 < rect.width && 0 < rect.height) {
		verify(Ellipse(_handle, rect.left(), rect.top(), rect.right(), rect.bottom()));
		_closed = true;
	}
}


void GraphicsPath::addEllipse(int x, int y, int width, int height) {
	addEllipse(Rectangle(x, y, width, height));
}


void GraphicsPath::addLine(const Point& p0, const Point& p1) {
	const Point points[] = { p0, p1 };
	addLines(points);
}


void GraphicsPath::addLine(int x0, int y0, int x1, int y1) {
	const Point points[] = { Point(x0, y0), Point(x1, y1) };
	addLines(points);
}


void GraphicsPath::addLines(ArrayRange<const Point> points) {
	assert("Null GraphicsPath handle" && _handle);
	assert("Invalid points size" && 2 <= points.size());
	if (_closed) {
		verify(MoveToEx(_handle, points[0].x, points[0].y, nullptr));
		points = ArrayRange<const Point>(points.begin() + 1, points.size());
	}
	verify(PolylineTo(_handle, reinterpret_cast<const POINT*>(points.begin()), static_cast<DWORD>(points.size())));
	_closed = false;
}


void GraphicsPath::addPath(const GraphicsPath& path) {
	assert("Null GraphicsPath handle" && _handle);

	Data data = path.data();
	if (0 < data.points.size()) {
		verify(PolyDraw(_handle, reinterpret_cast<const POINT*>(data.points.data()), data.types.data(), data.points.size()));
		_closed = true;
	}
}


void GraphicsPath::addPie(const Rectangle& rect, float startAngle, float sweepAngle) {
	assert("Null GraphicsPath handle" && _handle);

	if (rect.width <= 0 || rect.height <= 0) {
		return;
	}
	const float absSweepAngle = abs(sweepAngle);
	if (absSweepAngle < FLT_EPSILON) {
		return;
	}
	if (360.f - FLT_EPSILON < absSweepAngle) {
		addEllipse(rect);
	}
	else {
		Point start, end;
		int oldArcDirection = SetArcDirection(_handle, 0.f < sweepAngle ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);
		assert(oldArcDirection);
		calculateArcStartAndEnd(rect, startAngle, sweepAngle, start, end);
		verify(Pie(_handle, rect.x, rect.y, rect.right(), rect.bottom(), start.x, start.y, end.x, end.y));
		verify(SetArcDirection(_handle, oldArcDirection));
		_closed = true;
	}
}


void GraphicsPath::addPie(int x, int y, int width, int height, float startAngle, float sweepAngle) {
	addPie(Rectangle(x, y, width, height), startAngle, sweepAngle);
}


void GraphicsPath::addPolygon(ArrayRange<const Point> points) {
	assert("Null GraphicsPath handle" && _handle);
	assert("Invalid points size" && 2 <= points.size());

	verify(Polygon(_handle, reinterpret_cast<const POINT*>(points.begin()), static_cast<DWORD>(points.size())));
	_closed = true;
}


void GraphicsPath::addRectangle(const Rectangle& rect) {
	assert("Null GraphicsPath handle" && _handle);

	if (0 < rect.width && 0 < rect.height) {
		verify(::Rectangle(_handle, rect.left(), rect.top(), rect.right(), rect.bottom()));
		_closed = true;
	}
}


void GraphicsPath::addRectangle(int x, int y, int width, int height) {
	addRectangle(Rectangle(x, y, width, height));
}


void GraphicsPath::addRectangles(ArrayRange<const Rectangle> rects) {
	assert("Null GraphicsPath handle" && _handle);

	for (auto i = rects.begin(), end = rects.end(); i != end; ++i) {
		if (0 < i->width && 0 < i->height) {
			verify(::Rectangle(_handle, i->left(), i->top(), i->right(), i->bottom()));
			_closed = true;
		}
	}
}


void GraphicsPath::addText(StringRange s, const Point& point) {
	addText(s, point.x, point.y);
}


void GraphicsPath::addText(StringRange s, int x, int y) {
	assert("Null GraphicsPath handle" && _handle);

	int oldBkMode = SetBkMode(_handle, TRANSPARENT);
	assert(oldBkMode);
	verify(TextOutW(_handle, x, y, s.c_str(), static_cast<int>(s.length())));
	verify(SetBkMode(_handle, oldBkMode));
	_closed = true;
}


void GraphicsPath::addText(StringRange s, const Rectangle& rect, Graphics::TextFormat format) {
	assert("Null GraphicsPath handle" && _handle);
	assert("Invalid Graphics::TextFormat" && Graphics::TextFormat::_validate(format));

	int oldBkMode = SetBkMode(_handle, TRANSPARENT);
	assert(oldBkMode);
	RECT temp = rect;
	verify(DrawTextExW(_handle, const_cast<wchar_t*>(s.c_str()), static_cast<int>(s.length()), &temp, format, nullptr));
	verify(SetBkMode(_handle, oldBkMode));
	_closed = true;
}


Rectangle GraphicsPath::bounds() const {
	assert("Null GraphicsPath handle" && _handle);
	Region region = Region::fromGraphicsPath(*this);
	return region.bounds();
}


GraphicsPath GraphicsPath::clone() const {
	GraphicsPath path;
	path.addPath(*this);
	return path;
}


void GraphicsPath::closeFigure() {
	assert("Null GraphicsPath handle" && _handle);

	if (!_closed) {
		verify(CloseFigure(_handle));
	}
	_closed = true;
}


GraphicsPath::Data GraphicsPath::data() const {
	assert("Null GraphicsPath handle" && _handle);

	verify(EndPath(_handle));
	Data data;
	getPathData(_handle, data);
	return data;
}


void GraphicsPath::flatten() {
	assert("Null GraphicsPath handle" && _handle);

	verify(EndPath(_handle));
	verify(FlattenPath(_handle));
	Data data;
	getPathData(_handle, data); // 一度データ化して書き戻す
}


Font GraphicsPath::font() const {
	assert("Null GraphicsPath handle" && _handle);
	auto font = static_cast<HFONT>(GetCurrentObject(_handle, OBJ_FONT));
	assert("Failed to GetCurrentObject" && font);
	return Font(font);
}


Font GraphicsPath::font(HFONT value) {
	assert("Null GraphicsPath handle" && _handle);
	auto font = static_cast<HFONT>(SelectObject(_handle, value));
	assert("Failed to SelectObject" && font);
	return Font(font);
}


bool GraphicsPath::isVisible(const Point& point) const {
	return isVisible(point.x, point.y);
}


bool GraphicsPath::isVisible(int x, int y) const {
	Region region = Region::fromGraphicsPath(*this);
	return region.isVisible(x, y);
}


Point GraphicsPath::lastPoint() const {
	assert("Null GraphicsPath handle" && _handle);
	POINT point;
	verify(GetCurrentPositionEx(_handle, &point));
	return Point(point);
}


void GraphicsPath::reset() {
	assert("Null GraphicsPath handle" && _handle);

	verify(EndPath(_handle));
	verify(BeginPath(_handle));
	_closed = true;
}


void GraphicsPath::widen(HPEN pen) {
	assert("Null GraphicsPath handle" && _handle);
	assert("Null pen" && pen);

	const HPEN oldPen = static_cast<HPEN>(SelectObject(_handle, pen)); // 1. 太いペンを設定
	assert(oldPen);
	Data data;
	verify(EndPath(_handle));
	getPathData(_handle, data);            // 2.太いペンで書き直す
	verify(SelectObject(_handle, oldPen)); // 3.ペンを元に戻す
	verify(EndPath(_handle));
	BOOL result = WidenPath(_handle);      // 4.太くなったパスを塗りつぶし領域に変換
	getPathData(_handle, data);            // 5.変換した領域を書き直す

	if (!result) { // 失敗したのならおそらくコスメティックペンだったのでジオメトリックペンで書き直す
		int size = GetObjectW(pen, 0, nullptr);
		assert("Failed to WidenPath" && sizeof(EXTLOGPEN) < size); // ジオメトリックペンなのに失敗した？（ユーザダッシュパターンの指定はジオメトリックペンのみ）
		EXTLOGPEN info;
		ZeroMemory(&info, sizeof(info));
		verify(GetObjectW(pen, size, &info));
		assert("Failed to WidenPath" && (info.elpPenStyle & PS_GEOMETRIC) != 0); // ジオメトリックペンなのに失敗した？
		info.elpPenStyle |= PS_GEOMETRIC;

		LOGBRUSH logbrush;
		logbrush.lbStyle = info.elpBrushStyle;
		logbrush.lbColor = info.elpColor;
		logbrush.lbHatch = info.elpHatch;
		HPEN geometricPen = ExtCreatePen(info.elpPenStyle, info.elpWidth, &logbrush, 0, nullptr);
		assert(geometricPen);
		scopeExit([&]() {
			verify(DeleteObject(geometricPen));
		});
		widen(geometricPen);
	}
}



	}
}