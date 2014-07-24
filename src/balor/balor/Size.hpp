#pragma once


namespace balor {

class Point;



/**
 * 幅と高さによる寸?を?す?造体。
 */
class Size {
public:
	/// 初期化せずに作成。
	Size() {}
	Size(const Size& size);
	/// Point ?造体で初期化。
	Size(const Point& point);
	/// 幅と高さで初期化。
	Size(int width, int height);

public:
	/// 要素ごとに最小値を求める。
	static Size minimize(const Size& lhs, const Size& rhs);
	/// 要素ごとに最大値を求める。
	static Size maximize(const Size& lhs, const Size& rhs);
	/// 要素に負数が含まれるかどうか
	bool negative() const;

public:
	/// Point ?造体への変換。
	operator Point() const;
	Size& operator += (const Size& rhs);
	friend Size operator + (const Size& lhs, const Size& rhs);
	Size& operator -= (const Size& rhs);
	friend Size operator - (const Size& lhs, const Size& rhs);
	Size& operator *= (int rhs);
	friend Size operator * (const Size& lhs, int rhs);
	Size& operator /= (int rhs);
	friend Size operator / (const Size& lhs, int rhs);
	bool operator == (const Size& rhs) const;
	bool operator != (const Size& rhs) const;

public:
	int width;
	int height;
};



}