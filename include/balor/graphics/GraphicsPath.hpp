#pragma once

#include <vector>

#include <balor/graphics/Graphics.hpp>
#include <balor/ArrayRange.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct HDC__;
struct HFONT__;
struct HPEN__;

namespace balor {
class Point;
class Rectangle;
class Size;
}


namespace balor {
	namespace graphics {



/**
 * 描画した一連の線の集合を表す。
 *
 * GraphicsPath は Graphics クラスの関数と図形の描画結果が微妙に異なるので１ドットの違いも許せない場合には使用すべきではない。
 * Graphics::drawPath した場合は Graphics::draw～系関数と結果がだいたい一致するが、Graphics::fillPath した場合は
 * 図形の右下ピクセルを除いた形になるか、図形によっては輪郭部分を除いた形になる。
 * また GraphicsPath から Region を作成した場合は主に fillPath した場合の領域が作成される。
 */
class GraphicsPath : private NonCopyable {
public:
	typedef ::HDC__* HDC;
	typedef ::HFONT__* HFONT;
	typedef ::HPEN__* HPEN;

	/// 描画した線のデータ。
	class Data {
	public:
		Data() {}
		Data(Data&& value);

		Data& operator=(Data&& value);

	public:
		std::vector<Point> points;
		std::vector<unsigned char> types;
	};


public:
	/// まっさらな描画状態で作成。
	GraphicsPath();
	GraphicsPath(GraphicsPath&& value);
	/// 図形データから作成。
	explicit GraphicsPath(const Data& data);
	~GraphicsPath();

	GraphicsPath& operator=(GraphicsPath&& value);

public:
	/// 円弧を追加する。
	void addArc(const Rectangle& rect, float startAngle, float sweepAngle);
	void addArc(int x, int y, int width, int height, float startAngle, float sweepAngle);
	/// ベジェ曲線を追加する。終点のピクセルは描画されない。
	void addBezier(const Point& p0, const Point& p1, const Point& p2, const Point& p3);
	void addBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
	/// ベジェ曲線を連続で追加する。終点のピクセルは描画されない。
	void addBeziers(ArrayRange<const Point> points);
	/// 円を追加する。
	void addCircle(const Point& point, int radius);
	void addCircle(int x, int y, int radius);
	/// 楕円を追加する。
	void addEllipse(const Rectangle& rect);
	void addEllipse(int x, int y, int width, int height);
	/// 線分を追加する。終点のピクセルは描画されない。
	void addLine(const Point& p0, const Point& p1);
	void addLine(int x0, int y0, int x1, int y1);
	/// 線分を連続で追加する。終点のピクセルは描画されない。
	void addLines(ArrayRange<const Point> points);
	/// パスを追加する。
	void addPath(const GraphicsPath& path);
	/// 扇形を追加する。
	void addPie(const Rectangle& rectangle, float startAngle, float sweepAngle);
	void addPie(int x, int y, int width, int height, float startAngle, float sweepAngle);
	/// 多角形を追加する。
	void addPolygon(ArrayRange<const Point> points);
	/// 長方形を追加する。
	void addRectangle(const Rectangle& rect);
	void addRectangle(int x, int y, int width, int height);
	/// 長方形を連続で追加する。
	void addRectangles(ArrayRange<const Rectangle> rects);
	/// 文字列を追加する。
	void addText(StringRange s, const Point& point);
	void addText(StringRange s, int x, int y);
	void addText(StringRange s, const Rectangle& rect, Graphics::TextFormat format = Graphics::TextFormat::default);
	/// 図形を含む矩形。
	Rectangle bounds() const;
	/// 複製を返す。
	GraphicsPath clone() const;
	/// 直前に描いた図形を閉じる。
	void closeFigure();
	/// 図形データ。
	Data data() const;
	/// 曲線データを直線に分解する。
	void flatten();
	/// 文字を描画するフォント。
	Font font() const;
	Font font(HFONT value);
	/// HDC を返す。
	HDC hdc() const { return _handle; }
	/// 点が図形に含まれるかどうか。
	bool isVisible(const Point& point) const;
	bool isVisible(int x, int y) const;
	/// 最後の図形を書き終わった点。
	Point lastPoint() const;
	/// 図形を全て消す。
	void reset();
	/// 図形の線を指定したペンの太さにする。太くした線自体が図形になる。
	void widen(HPEN pen);

private:
	HDC _handle;
	bool _closed;
};



	}
}