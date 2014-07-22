#pragma once


namespace balor {
class Size;
}

		
namespace balor {
	namespace gui {



/**
 * 上下左右の余白を表す。
 */
class Padding {
public:
	/// 初期化せずに作成。
	Padding() {}
	Padding(const Padding& value);
	/// 全て同じ数値で初期化。
	Padding(int all);
	/// それぞれの数値で初期化。
	Padding(int left, int top, int right, int bottom);

public:
	/// 水平方向の余白の合計。
	int horizontal() const;
	/// 余白の合計の大きさ。
	Size size() const;
	/// 垂直方向の余白の合計。
	int vertical() const;

public:
	Padding& operator += (const Padding& rhs);
	friend Padding operator + (const Padding& lhs, const Padding& rhs);
	Padding& operator -= (const Padding& rhs);
	friend Padding operator - (const Padding& lhs, const Padding& rhs);
	bool operator == (const Padding& rhs) const;
	bool operator != (const Padding& rhs) const;

public:
	int left; // 左の余白
	int top; // 上の余白
	int right; // 右の余白
	int bottom; // 下の余白
};



	}
}