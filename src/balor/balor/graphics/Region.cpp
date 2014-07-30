#include "Region.hpp"

#include <utility>

#include <balor/graphics/GraphicsPath.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/Size.hpp>


namespace balor {
	namespace graphics {

using std::move;
using std::swap;


namespace {
static_assert(Region::Operation::and  == RGN_AND, "Invalid enum value");
static_assert(Region::Operation::or   == RGN_OR, "Invalid enum value");
static_assert(Region::Operation::xor  == RGN_XOR, "Invalid enum value");
static_assert(Region::Operation::diff == RGN_DIFF, "Invalid enum value");

const int infinitSize = 0x400000;

inline Rectangle getInfiniteBounds() {
	return Rectangle::fromLTRB(-infinitSize, -infinitSize, infinitSize, infinitSize);
}
} // namespace



bool Region::Operation::_validate(Operation value) {
	return and <= value && value <= diff;
}


Region::Region() : _handle(nullptr), _owned(false) {
}


Region::Region(Region&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Region::Region(HRGN handle, bool owned) : _handle(handle), _owned(owned) {
}


Region::Region(const Rectangle& rect) : _owned(true) {
	_handle = CreateRectRgn(rect.left(), rect.top(), rect.right(), rect.bottom());
	assert("Failed to CreateRectRgn" && _handle);
}


Region::Region(int x, int y, int width, int height) : _handle(nullptr), _owned(false) {
	*this = Region(Rectangle(x, y, width, height));
}


Region::~Region() {
	if (_handle && _owned) {
		verify(DeleteObject(_handle));
	}
	//_handle = nullptr;
}


Region& Region::operator=(Region&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


Rectangle Region::bounds() const {
	if (!*this) {
		return getInfiniteBounds();
	}

	RECT rect;
	verify(GetRgnBox(_handle, &rect));
	return Rectangle(rect);
}


Region Region::clone() const {
	if (!*this) {
		return Region();
	}
	return clone(_handle);
}


Region Region::clone(HRGN handle) {
	assert("Null handle" && handle);
	Region region;
	region.makeInfinite();
	verify(CombineRgn(region, handle, nullptr, RGN_COPY) != ERROR);
	return region;
}


void Region::combine(HRGN rhs, Region::Operation op) {
	assert("Invalid Region::Operation" && Operation::_validate(op));

	if (!*this) {
		makeInfinite();
	}
	Region rtemp;
	if (!rhs) {
		rtemp.makeInfinite();
		rhs = rtemp;
	}
	verify(CombineRgn(_handle, _handle, rhs, op) != ERROR);
}


Region Region::combine(HRGN lhs, HRGN rhs, Region::Operation op) {
	assert("Invalid Region::Operation" && Operation::_validate(op));

	Region ltemp;
	if (!lhs) {
		ltemp.makeInfinite();
		lhs = ltemp;
	}
	Region rtemp;
	if (!rhs) {
		rtemp.makeInfinite();
		rhs = rtemp;
	}
	Region result;
	result.makeInfinite();
	verify(CombineRgn(result, lhs, rhs, op) != ERROR);
	return result;
}


bool Region::empty() const {
	if (!*this) {
		return false;
	}

	RECT rect;
	const int result = GetRgnBox(_handle, &rect);
	assert(result);
	return result == NULLREGION;
}


bool Region::equals(HRGN rhs) const {
	if (!*this && !rhs) {
		return true;
	}

	HRGN lhs = _handle;
	Region ltemp;
	if (!lhs) {
		ltemp.makeInfinite();
		lhs = ltemp;
	}
	Region rtemp;
	if (!rhs) {
		rtemp.makeInfinite();
		rhs = rtemp;
	}
	const BOOL result = EqualRgn(lhs, rhs);
	return result != 0;
}


Region Region::fromCircle(const Point& point, int radius) {
	return fromEllipse(Rectangle(point.x - radius, point.y - radius, radius * 2, radius * 2));
}


Region Region::fromCircle(int x, int y, int radius) {
	return fromEllipse(Rectangle(x - radius, y - radius, radius * 2, radius * 2));
}


Region Region::fromEllipse(const Rectangle& rect) {
	Region region;
	region._handle = CreateEllipticRgn(rect.left(), rect.top(), rect.right() + 1, rect.bottom() + 1);
	assert("Failed to CreateEllipticRgn" && region._handle);
	region._owned = true;
	return region;
}


Region Region::fromEllipse(int x, int y, int width, int height) {
	return fromEllipse(Rectangle(x, y, width, height));
}


Region Region::fromGraphicsPath(const GraphicsPath& path) {
	GraphicsPath tempPath = path.clone();
	verify(EndPath(tempPath.hdc()));
	Region region;
	region._handle = PathToRegion(tempPath.hdc());
	assert("Failed to PathToRegion" && region._handle);
	region._owned = true;
	return region;
}


Region Region::fromPolygon(ArrayRange<const Point> points, bool alternate) {
	assert("Invalid points size" && 2 <= points.size());

	Region region;
	region._handle = CreatePolygonRgn(reinterpret_cast<const POINT*>(points.begin()), points.size(), alternate ? ALTERNATE : WINDING);
	assert("Failed to CreatePolygonRgn" && region._handle);
	region._owned = true;
	return region;
}


Region Region::fromRectangle(const Rectangle& rect) {
	Region region;
	region._handle = CreateRectRgn(rect.left(), rect.top(), rect.right(), rect.bottom());
	assert("Failed to CreateRectRgn" && region._handle);
	region._owned = true;
	return region;
}


Region Region::fromRectangle(int x, int y, int width, int height) {
	return fromRectangle(Rectangle(x, y, width, height));
}


Region Region::fromRoundRectangle(const Rectangle& rect, const Size& ellipseSize) {
	Region region;
	region._handle = CreateRoundRectRgn(rect.left(), rect.top(), rect.right() + 1, rect.bottom() + 1, ellipseSize.width, ellipseSize.height);
	assert("Failed to CreateRoundRectRgn" && region._handle);
	region._owned = true;
	return region;
}


Region Region::fromRoundRectangle(int x, int y, int width, int height, int ellipseWidth, int ellipseHeight) {
	return fromRoundRectangle(Rectangle(x, y, width, height), Size(ellipseWidth, ellipseHeight));
}


bool Region::infinite() const {
	if (!*this) {
		return true;
	}

	RECT rect;
	int result = GetRgnBox(_handle, &rect);
	assert(result);
	if (result != SIMPLEREGION) {
		return false;
	}
	return Rectangle(rect).contains(getInfiniteBounds());
}


bool Region::isVisible(const Point& point) const {
	return isVisible(point.x, point.y);
}


bool Region::isVisible(const Rectangle& rect) const {
	if (!*this) {
		return true;
	}
	const RECT temp = rect;
	return RectInRegion(_handle, &temp) != 0;
}


bool Region::isVisible(int x, int y) const {
	if (!*this) {
		return true;
	}
	return PtInRegion(_handle, x, y) != 0;
}


bool Region::isVisible(int x, int y, int width, int height) const {
	return isVisible(Rectangle(x, y, width, height));
}


void Region::makeEmpty() {
	*this = Region(Rectangle(0, 0, 0, 0));
}


void Region::makeInfinite() {
	*this = Region(getInfiniteBounds());
}


bool Region::owned() const {
	return _owned;
}


void Region::owned(bool value) {
	_owned = value;
}


void Region::translate(int x, int y) {
	if (*this) {
		verify(OffsetRgn(_handle, x, y) != ERROR);
	}
}



	}
}