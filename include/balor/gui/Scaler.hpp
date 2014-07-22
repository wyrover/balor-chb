#pragma once

#include <balor/Size.hpp>

struct HFONT__;

namespace balor {
	class Point;
	class Rectangle;
}


namespace balor {
	namespace gui {

class Padding;



/**
 * 数値のスケーリングを行う。
 *
 * 画面解像度（ＤＰＩ）等を元にコントロールをスケーリングするのに使う。
 */
class Scaler {
public:
	typedef ::HFONT__* HFONT;

public:
	/// defaultDpi() から現在の画面の DPI へのスケーリングを行うように作成。
	Scaler();
	/// from から to へのスケーリングを行うように作成。Size の width, height はそれぞれ水平方向と垂直方向のスケーリングにかかわる。
	/// 例えば下記のように作成すると縦横方向に２倍のスケーリングを行う。<br><br>
	///  Scale(Size(100, 100), Size(200, 200))
	Scaler(const Size& from, const Size& to);
	/// 縦横比率を等しいものとして from から to へのスケーリングを作成。
	Scaler(int from, int to);
	/// フォントの平均文字幅と高さからスケーリングを行うように作成。
	Scaler(HFONT from, HFONT to);

public:
	/// デフォルトコンストラクタで from に使用する DPI。初期値は Size(96, 96)。この値は Control やその派生クラスの内部でも使われているので
	/// 特に開発環境の DPI が 96 ではない場合はコントロールを作成する前に開発環境の DPI を設定する必要がある。
	static Size defaultDpi();
	static void defaultDpi(const Size& value);
	/// 水平方向の倍率でスケーリングする。
	int scale(int value) const;
	double scale(double value) const;
	/// 水平方向の倍率でスケーリングする。
	int scaleH(int value) const;
	double scaleH(double value) const;
	/// 垂直方向の倍率でスケーリングする。
	int scaleV(int value) const;
	double scaleV(double value) const;
	/// スケーリングする。
	Padding scale(const Padding& value) const;
	Point scale(const Point& value) const;
	Rectangle scale(const Rectangle& value) const;
	Size scale(const Size& value) const;

private:
	Size _from;
	Size _to;
};



	}
}