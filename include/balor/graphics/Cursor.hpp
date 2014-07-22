#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct HBITMAP__;
struct HICON__;

namespace balor {
	namespace io {
		class Stream;
	}
	class Point;
	class Size;
}


namespace balor {
	namespace graphics {

class Bitmap;
class Color;



/**
 * カーソルの画像を表す。
 */
class Cursor : private NonCopyable {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::HICON__* HCURSOR;
	typedef ::balor::io::Stream Stream;

	/// ファイルが見つからなかった。
	class NotFoundException : public Exception {};

	/// ロードに失敗した。
	class LoadFailedException : public Exception {};

public:
	/// ヌルハンドルで作成。
	Cursor();
	Cursor(Cursor&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Cursor(HCURSOR handle, bool owned = false);
	/// 画像とマスク画像とホットスポットから作成。マスク画像はモノクロビットマップで白い部分を透過する。
	/// bitmap が 32 ビット画像でかつ１ドットでも０ではないアルファを持つならマスク画像を無視してアルファチャンネルで透過する。
	Cursor(HBITMAP bitmap, HBITMAP mask, int xHotSpot, int yHotSpot);
	/// 画像とカラーキーとホットスポットから作成。カラーキーの色の部分を透過する。
	/// bitmap が 32 ビット画像でかつ１ドットでも０ではないアルファを持つならカラーキーを無視してアルファチャンネルで透過する。
	Cursor(HBITMAP bitmap, const Color& colorKey, int xHotSpot, int yHotSpot);
	/// ファイルから作成。.cur、 .ani ファイルに対応。
	explicit Cursor(StringRange filePath);
	~Cursor();
	Cursor& operator=(Cursor&& value);

public:
	/// カーソルの画像。
	Bitmap bitmap() const;
	/// 複製を返す。
	Cursor clone() const;
	static Cursor clone(HCURSOR handle);
	/// 標準のサイズ。
	static Size defaultSize();
	/// カーソルがポイントする点。画像の左上を原点とするピクセル位置。
	Point hotSpot();
	/// カーソルの背景を透過するモノクロビットマップ。白い部分を透過する。
	Bitmap mask() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// ストリームに保存する。
	void save(Stream&& stream) const;
	void save(Stream& stream) const;
	/// ファイルに保存する。
	void save(StringRange filePath) const;
	/// 複数のカーソルを一つのファイルにまとめてストリームに保存する。
	static void save(ArrayRange<const Cursor> cursors, Stream&& stream);
	static void save(ArrayRange<const Cursor> cursors, Stream& stream);
	/// 複数のカーソルを一つのファイルにまとめてファイルに保存する。
	static void save(ArrayRange<const Cursor> cursors, StringRange filePath);
	/// カーソルの大きさ。
	Size size();

public: // システムカーソル一覧。
	/// 矢印カーソルと砂時計。
	static Cursor appStarting();
	/// 矢印カーソル。
	static Cursor default();
	/// 十字カーソル。
	static Cursor cross();
	/// ハンドカーソル。
	static Cursor hand();
	/// 矢印カーソルと疑問符。
	static Cursor help();
	/// 縦線カーソル。
	static Cursor iBeam();
	/// 禁止カーソル。
	static Cursor no();
	/// ４方向の矢印カーソル。
	static Cursor sizeAll();
	/// 右上と左下方向の矢印カーソル。
	static Cursor sizeNESW();
	/// 上下方向の矢印カーソル。
	static Cursor sizeNS();
	/// 左上と右下方向の矢印カーソル。
	static Cursor sizeNWSE();
	/// 左右方向の矢印カーソル。
	static Cursor sizeWE();
	/// 上方向の矢印カーソル。
	static Cursor upAllow();
	/// 砂時計カーソル。
	static Cursor wait();

public:
	/// HCURSOR への自動変換 ＆ null チェック用
	operator HCURSOR() const { return _handle; }

private:
	HCURSOR _handle;
	bool _owned;
};



	}
}