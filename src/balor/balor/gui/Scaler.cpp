#include "Scaler.hpp"

#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Padding.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/Singleton.hpp>


namespace balor {
	namespace gui {

using namespace balor::graphics;


namespace {
class Global {
	friend Singleton<Global>;

	Global() : defaultDpi(96, 96) {
	}

	~Global() {
	}

public:
	Size defaultDpi;
};
}// namespace



Scaler::Scaler() : _from(defaultDpi()) {
	struct CurrentDPI {
		CurrentDPI() {
			Graphics graphics = Graphics::fromGraphics(nullptr);
			dpi.width  = graphics.dpiX();
			dpi.height = graphics.dpiY();
		}

		Size dpi;
	};
	static CurrentDPI dpi; // DLL Ç≤Ç∆Ç…é¿ëÃÇéùÇ∆Ç§Ç™èdï°èâä˙âªÇ≥ÇÍÇÊÇ§Ç™ñ‚ëËÇ»Ç¢
	_to = dpi.dpi;
}


Scaler::Scaler(const Size& from, const Size& to) : _from(from), _to(to) {
}


Scaler::Scaler(int from, int to) : _from(from, from), _to(to, to) {
}


Scaler::Scaler(HFONT from, HFONT to) {
	Graphics graphics = Graphics::fromGraphics(nullptr);
	auto font = graphics.font(from);
	_from.width  = Font(from).averageWidth();
	_from.height = graphics.measureText(L"0").height;
	graphics.font(to);
	_to.width    = Font(to  ).averageWidth();
	_to.height   = graphics.measureText(L"0").height;
	graphics.font(font);
}


Size Scaler::defaultDpi() {
	return Singleton<Global>::get().defaultDpi;
}


void Scaler::defaultDpi(const Size& value) {
	Singleton<Global>::get().defaultDpi = value;
}


int Scaler::scale(int value) const {
	return MulDiv(value, _to.width, _from.width);
}


double Scaler::scale(double value) const {
	return value * _to.width / _from.width;
}


int Scaler::scaleH(int value) const {
	return MulDiv(value, _to.width, _from.width);
}


double Scaler::scaleH(double value) const {
	return value * _to.width / _from.width;
}


int Scaler::scaleV(int value) const {
	return MulDiv(value, _to.height, _from.height);
}


double Scaler::scaleV(double value) const {
	return value * _to.height / _from.height;
}


Padding Scaler::scale(const Padding& value) const {
	return Padding(MulDiv(value.left  , _to.width , _from.width )
				  ,MulDiv(value.top   , _to.height, _from.height)
				  ,MulDiv(value.right , _to.width , _from.width )
				  ,MulDiv(value.bottom, _to.height, _from.height));
}


Point Scaler::scale(const Point& value) const {
	return Point(MulDiv(value.x, _to.width , _from.width )
				,MulDiv(value.y, _to.height, _from.height));
}


Rectangle Scaler::scale(const Rectangle& value) const {
	return Rectangle(MulDiv(value.x     , _to.width , _from.width )
					,MulDiv(value.y     , _to.height, _from.height)
					,MulDiv(value.width , _to.width , _from.width )
					,MulDiv(value.height, _to.height, _from.height));
}


Size Scaler::scale(const Size& value) const {
	return Size(MulDiv(value.width , _to.width , _from.width )
			   ,MulDiv(value.height, _to.height, _from.height));
}



	}
}