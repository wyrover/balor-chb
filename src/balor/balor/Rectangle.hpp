#pragma once


struct tagRECT;


namespace balor {

class Size;
class Point;



/**
 * 平面上の位置と大きさを持つ領域を表す構造体。
 *
 * WIN32 の RECT 構造体と相互変換できる。
 * WIN32 の Rectangle 関数と名前がかぶるので windows.h と一緒に使う場合は別名の Rect を使うと良い。
 */
class Rectangle {
public:
	typedef ::tagRECT RECT;

public:
	/// 初期化せずに作成。
	Rectangle() {}
	Rectangle(const Rectangle& rectangle);
	/// RECT 構造体で初期化。
	Rectangle(const RECT& rect);
	/// 位置と大きさの構造体で初期化。
	Rectangle(const Point& position, const Size& size);
	/// 位置と大きさで初期化。
	Rectangle(int x, int y, int width, int height);

public:
	/// 領域の下端座標。
	int bottom() const { return y + height; }
	/// 領域の左下座標。
	Point bottomLeft() const;
	/// 領域の右下座標。
	Point bottomRight() const;
	/// 点がこの領域に含まれるかどうか。
	bool contains(const Point& point) const;
	bool contains(int x, int y) const;
	/// rect 全体がこの領域に含まれるかどうか。
	bool contains(const Rectangle& rect) const;
	/// left, top, right, bottom 座標系から作成する。
	static Rectangle fromLTRB(int left, int top, int right, int bottom);
	/// 指定した大きさ分、上下左右に拡大する。
	void grow(const Size& size);
	void grow(int width, int height);
	/// 二つの領域の交差部分を返す。
	static Rectangle intersection(const Rectangle& lhs, const Rectangle& rhs);
	/// 領域が交差するかどうか。
	bool intersects(const Rectangle& rhs) const;
	/// 領域の左端座標。
	int left() const { return x; }
	/// 二つの領域の結合部分を返す。
	static Rectangle or(const Rectangle& lhs, const Rectangle& rhs);
	/// 領域の左上座標。
	Point position() const;
	void position(const Point& value);
	void position(int x, int y);
	/// 領域の右端座標。
	int right() const { return x + width; }
	/// 領域の大きさ。
	Size size() const;
	void size(const Size& value);
	void size(int width, int height);
	/// 領域の上端座標。
	int top() const { return y; }
	/// 領域の左上座標。
	Point topLeft() const;
	/// 領域の右上座標。
	Point topRight() const;
	/// 座標の数値分移動する。
	void translate(const Point& point);
	void translate(int x, int y);

public:
	/// RECT 構造体への変換。
	operator RECT() const;
	bool operator == (const Rectangle& rhs) const;
	bool operator != (const Rectangle& rhs) const;

public:
	int x;
	int y;
	int width;
	int height;
};


/// Rectangle は WIN32 の関数名とかぶるので windows.h と一緒に使う場合はこの名前で
typedef Rectangle Rect;



}