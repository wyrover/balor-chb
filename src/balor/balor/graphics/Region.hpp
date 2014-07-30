#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>

struct HRGN__;
struct HWND__;

namespace balor {
class Point;
class Rectangle;
class Size;
}


namespace balor {
	namespace graphics {

class GraphicsPath;



/**
* GDI リージョンを表す。
*
* リージョンのハンドルがヌルの場合は無限大の領域を表す。Graphics や ウインドウにリージョンが設定されていない場合はヌルハンドルを返すのでこのほうが都合が良い。
* from～ 関数によって作成された領域は Graphics::fill～ 関数と範囲が完全に一致する。
* 一方 GraphicsPath を介して図形を作った場合は Graphics のどの図形描画関数とも結果が一致しないので注意すること。
*/
class Region : private NonCopyable {
public:
	typedef ::HRGN__* HRGN;
	typedef ::HWND__* HWND;

	/// リージョンの合成演算子。
	struct Operation {
		enum _enum {
			and = 1, /// AND 合成。二つの領域の共通部分。
			or = 2, /// OR 合成。二つの領域をあわせた部分。
			xor = 3, /// XOR 合成。二つの領域をあわせた部分から共通部分を除いた部分。
			diff = 4, /// 左辺領域から右辺領域を取り除いた部分。
		};
		BALOR_NAMED_ENUM_MEMBERS(Operation);
	};

public:
	/// 無限大の領域として作成。
	Region();
	Region(Region&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Region(HRGN handle, bool owned = false);
	/// 長方形領域から作成。
	explicit Region(const Rectangle& rect);
	Region(int x, int y, int width, int height);
	~Region();
	Region& operator=(Region&& value);

public:
	/// 領域全体を含む長方形。
	Rectangle bounds() const;
	/// 複製して返す。
	Region clone() const;
	static Region clone(HRGN handle);
	/// 二つの領域を指定した演算で合成する。リージョンのハンドルがヌルの場合は無限の広さの領域として扱われる。
	void combine(HRGN rhs, Region::Operation op);
	static Region combine(HRGN lhs, HRGN rhs, Region::Operation op);
	/// 領域の面積が０かどうか。ハンドルがヌルかどうかではないことに注意。
	bool empty() const;
	/// 領域が等しいかどうか。operator == は operator HRGN() によってハンドルの比較になるので注意すること。
	bool equals(HRGN rhs) const;
	/// 円の領域を作成。
	static Region fromCircle(const Point& point, int radius);
	static Region fromCircle(int x, int y, int radius);
	/// 楕円の領域を作成。
	static Region fromEllipse(const Rectangle& rect);
	static Region fromEllipse(int x, int y, int width, int height);
	/// GraphicsPath に囲まれた領域を作成。
	static Region fromGraphicsPath(const GraphicsPath& path);
	/// 多角形の領域を作成。alternate 引数は塗りつぶしモードが ALTERNATE かどうかを表す。false の場合は WINDING となる。
	static Region fromPolygon(ArrayRange<const Point> points, bool alternate = true);
	/// 長方形の領域を作成。
	static Region fromRectangle(const Rectangle& rect);
	static Region fromRectangle(int x, int y, int width, int height);
	/// 角の丸い長方形の領域を作成。
	static Region fromRoundRectangle(const Rectangle& rect, const Size& ellipseSize);
	static Region fromRoundRectangle(int x, int y, int width, int height, int ellipseWidth, int ellipseHeight);
	/// 領域の面積が極大かどうか。ハンドルがヌルの場合も true。
	bool infinite() const;
	/// 点が領域に含まれるかどうか。
	bool isVisible(const Point& point) const;
	bool isVisible(int x, int y) const;
	/// 長方形全体が領域に含まれるかどうか。
	bool isVisible(const Rectangle& rect) const;
	bool isVisible(int x, int y, int width, int height) const;
	/// 領域の面積を０にする。
	void makeEmpty();
	/// 領域の面積を極大にする。
	void makeInfinite();
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// 領域を平行移動する。
	void translate(int dx, int dy);

public:
	/// HRGN への自動変換 ＆ null チェック用
	operator HRGN() const { return _handle; }

private:
	HRGN _handle;
	bool _owned;
};



	}
}