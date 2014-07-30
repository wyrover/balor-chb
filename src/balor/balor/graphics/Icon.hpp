#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/Size.hpp>
#include <balor/StringRange.hpp>

struct HBITMAP__;
struct HICON__;

namespace balor {
	namespace io {
		class Stream;
	}
}


namespace balor {
	namespace graphics {

class Bitmap;
class Color;



/**
* アイコンを表す。
*
* TODO: アイコンのストリーム読み込み。
* TODO: システムアイコンはどんなサイズを指定しても標準サイズになってしまう。
*/
class Icon : private NonCopyable {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::HICON__* HICON;
	typedef ::balor::io::Stream Stream;

	/// ファイルが見つからなかった。
	class NotFoundException : public Exception {};

	/// ロードに失敗した。
	class LoadFailedException : public Exception {};

public:
	/// ヌルハンドルで作成。
	Icon();
	Icon(Icon&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Icon(HICON handle, bool owned = false);
	/// 画像とマスク画像から作成。マスク画像はモノクロビットマップで白い部分を透過する。作成後、画像ハンドルは参照されない。
	/// bitmap が 32 ビット画像でかつ１ドットでも０ではないアルファを持つならマスク画像を無視してアルファチャンネルで透過する。
	Icon(HBITMAP bitmap, HBITMAP mask);
	/// 画像とカラーキーから作成。カラーキーの色の部分を透過する。作成後、画像ハンドルは参照されない。
	/// bitmap が 32 ビット画像でかつ１ドットでも０ではないアルファを持つならカラーキーを無視してアルファチャンネルで透過する。
	Icon(HBITMAP bitmap, const Color& colorKey);
	/// ストリームとサイズを指定して作成。size に Size(0, 0) を指定すると最初に見つかったアイコンの原寸で作成される。
	/*explicit Icon(Stream&& stream, const Size& size = Icon::largeSize());
	explicit Icon(Stream& stream, const Size& size = Icon::largeSize());
	Icon(Stream&& stream, int width, int height);
	Icon(Stream& stream, int width, int height); *//**/
	/// ファイル名とサイズを指定して作成。size に Size(0, 0) を指定すると最初に見つかったアイコンの原寸で作成される。
	explicit Icon(StringRange filePath, const Size& size = Icon::largeSize());
	Icon(StringRange filePath, int width, int height);
	~Icon();

	Icon& operator=(Icon&& value);

public:
	/// アイコンの画像。
	Bitmap bitmap() const;
	/// アイコンの複製を返す。
	Icon clone() const;
	static Icon clone(HICON handle);
	/// 大きなアイコンの標準サイズ。
	static Size largeSize();
	/// アイコンの背景を透過するモノクロビットマップ。白い部分を透過する。
	Bitmap mask() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// ストリームに保存する。
	void save(Stream&& stream) const;
	void save(Stream& stream) const;
	/// ファイルに保存する。
	void save(StringRange filePath) const;
	/// 複数のアイコンを一つのファイルにまとめてストリームに保存する。
	static void save(ArrayRange<const Icon> icons, Stream&& stream);
	static void save(ArrayRange<const Icon> icons, Stream& stream);
	/// 複数のアイコンを一つのファイルにまとめてファイルに保存する。
	static void save(ArrayRange<const Icon> icons, StringRange filePath);
	/// アイコンのサイズ。
	Size size() const;
	/// 小さなアイコンの標準サイズ。
	static Size smallSize();

public: // システムアイコン一覧。
	/// アプリケーションアイコン。
	static Icon application(const Size& size = Icon::largeSize());
	/// アスタリスクアイコン。
	static Icon asterisk(const Size& size = Icon::largeSize());
	/// エラーアイコン。
	static Icon error(const Size& size = Icon::largeSize());
	/// 感嘆符アイコン。
	static Icon exclamation(const Size& size = Icon::largeSize());
	/// 手の形のアイコン。
	static Icon hand(const Size& size = Icon::largeSize());
	/// 情報アイコン。
	static Icon information(const Size& size = Icon::largeSize());
	/// 疑問符アイコン。
	static Icon question(const Size& size = Icon::largeSize());
	/// 警告アイコン。
	static Icon warning(const Size& size = Icon::largeSize());
	/// Windos ロゴアイコン。
	static Icon windowsLogo(const Size& size = Icon::largeSize());

public:
	/// HICON への自動変換 ＆ null チェック用
	operator HICON() const { return _handle; }

private:
	HICON _handle;
	bool _owned;
};



	}
}