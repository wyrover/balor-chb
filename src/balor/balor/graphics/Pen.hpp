#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>

struct HBRUSH__;
struct HPEN__;


namespace balor {
	namespace graphics {

class Brush;
class Color;



/**
 * GDI ペンを表す。
 */
class Pen : private NonCopyable {
public:
	typedef ::HBRUSH__* HBRUSH;
	typedef ::HPEN__* HPEN;

	/// 線の始点と終点の形。
	struct LineCap {
		enum _enum {
			round  = 0x00000000, /// 線の端点を丸くする。
			square = 0x00000100, /// 線の端点を四角くする。
			flat   = 0x00000200, /// 線の端点を平らにする。square が端点に四角の半分を追加するのに対し flat は端点でカットする。
		};
		BALOR_NAMED_ENUM_MEMBERS(LineCap);
	};


	/// 線の接続部分の形。
	struct LineJoin {
		enum _enum {
			round  = 0x00000000, /// 接続部分を丸くする。
			bevel  = 0x00001000, /// 接続部分を平らにする。
			mitter = 0x00002000, /// 線と線の距離が Graphics::mitterLimit() 以下ならば接続部分を尖らせる。そうでなければ bevel と同じ。
		};
		BALOR_NAMED_ENUM_MEMBERS(LineJoin);
	};

	/// ペンの線のスタイル。
	struct Style {
		enum _enum {
			solid       = 0, /// 実線。
			dash        = 1, /// 破線。
			dot         = 2, /// 点線。
			dashDot     = 3, /// 一点鎖線。
			dashDotDot  = 4, /// 二点鎖線。
			null        = 5, /// 線を描画しない。
			insideFrame = 6, /// 実線で、図形を描く場合に線幅が図形内に完全に収まるように書く。
			custom      = 7, /// 線のパターンをユーザが設定する。
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};

public:
	/// ヌルハンドルで作成。
	Pen();
	Pen(Pen&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Pen(HPEN handle, bool owned = false);
	/// ブラシと幅とスタイル等から作成。作成後、ブラシハンドルは参照されない。
	explicit Pen(HBRUSH brush, int width = 1, Pen::Style style = Style::solid, LineCap lineCap = LineCap::square, LineJoin lineJoin = LineJoin::mitter);
	/// ブラシと幅と点線のパターン等から作成。作成後、ブラシハンドルは参照されない。
	Pen(HBRUSH brush, int width, ArrayRange<const int> patten, LineCap lineCap = LineCap::square, LineJoin lineJoin = LineJoin::mitter);
	/// 色と幅とスタイル等から作成。
	explicit Pen(const Color& color, int width = 1, Pen::Style style = Style::solid, LineCap lineCap = LineCap::square, LineJoin lineJoin = LineJoin::mitter);
	/// 色と幅と点線のパターン等から作成。
	Pen(const Color& color, int width, ArrayRange<const int> patten, LineCap lineCap = LineCap::square, LineJoin lineJoin = LineJoin::mitter);
	~Pen();

	Pen& operator=(Pen&& value);

public:
	/// 描画に使うブラシ。ブラシを使わない場合はヌルハンドルのブラシが返る。
	Brush brush() const;
	/// 複製を返す。
	Pen clone() const;
	static Pen clone(HPEN handle);
	/// 線の色。ブラシを使う場合は意味を持たない。
	Color color() const;
	/// 線の始点と終点の形。
	Pen::LineCap lineCap() const;
	/// 線の接続点の形。
	Pen::LineJoin lineJoin() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// 点線のパターン。コンストラクタで指定しなかった場合は空の配列。
	std::vector<int, std::allocator<int> > pattern() const;
	/// 点線のスタイル。
	Pen::Style style() const;
	/// 線の幅。
	int width() const;

public: // システムペン一覧
	/// 白い実線のペン。
	static Pen white();
	/// 黒い実線のペン。
	static Pen black();
	/// 描画しないペン。
	static Pen hollow();

public:
	/// HPEN への自動変換 ＆ null チェック用
	operator HPEN() const { return _handle; }

private:
	HPEN _handle;
	bool _owned;
};



	}
}