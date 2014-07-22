#pragma once

struct tagPOINT;


namespace balor {



/**
 * 平面の座標を表す構造体。
 * 
 * 各種演算子をサポートする他、WIN32 の POINT 構造体と相互変換できる。
 */
class Point {
public:
	typedef ::tagPOINT POINT;

public:
	/// 初期化せずに作成。
	Point() {}
	Point(const Point& value);
	/// POINT 構造体で初期化。
	Point(const POINT& point);
	/// 座標で初期化。
	Point(int x, int y);

public:
	/// 要素ごとに最大値を求める。
	static Point maximize(const Point& lhs, const Point& rhs);
	/// 要素ごとに最小値を求める。
	static Point minimize(const Point& lhs, const Point& rhs);

public:
	/// POINT 構造体への変換
	operator POINT() const;
	Point operator-() const;
	Point& operator += (const Point& rhs);
	friend Point operator + (const Point& lhs, const Point& rhs);
	Point& operator -= (const Point& rhs);
	friend Point operator - (const Point& lhs, const Point& rhs);
	bool operator == (const Point& rhs) const;
	bool operator != (const Point& rhs) const;

public:
	int x;
	int y;
};



}