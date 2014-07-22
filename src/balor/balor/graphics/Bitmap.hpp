#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/OutOfMemoryException.hpp>
#include <balor/StringRange.hpp>

struct HBITMAP__;

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
	namespace io {
		class Stream;
	}
	class Point;
	class Rectangle;
	class Size;
}


namespace balor {
	namespace graphics {

class Color;



/**
 * GDI ビットマップを表す。
 * 
 * 画像のコピーには Graphics::copy 関数を、色の取得には Graphics::getPixel 関数を使用できる。あるいは pixels 関数で自力で処理する。
 * ファイルやストリームへの入出力には GDI+ を使用し、bmp, gif, jpeg, png, tiff のファイル形式及びアルファチャンネル付き画像形式をサポートする。
 * コンストラクタで作成した場合は常に DIB ビットマップを作成する。DDB ビットマップを作成するには createDDB または toDDB 関数を使用する。
 * DDBは一定以上の大きさ（例えば4096 * 4096）で作成したり、一定量以上の容量（しかしメインメモリには十分入る）を超えると
 * OutOfMemoryException を発生させる。これは主にビデオカードの制限による。パレットを使用する DDB はサポートしない。
 * 引数に HBITMAP を渡す関数や bottomUp, palette, save 関数等は HBITMAP や Bitmap が Graphics クラスに参照されたままだと関数が失敗するので注意すること。
 * 
 * ※ DDB とはディスプレイと同じピクセルフォーマットを持ち、高速に画面に描画ができるが内容のメモリポインタが得られないビットマップで、
 *    DIB はオリジナルのフォーマットを持てる反面、画面に描画を行うと内部では変換処理が走るためにパフォーマンスが低下する恐れがあるビットマップ。
 */
class Bitmap : private NonCopyable {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::balor::io::Stream Stream;


	/// メモリが足りなかった。
	struct OutOfMemoryException : public ::balor::OutOfMemoryException {};

	/// ファイルフォーマットが不正。
	class FileFormatException : public Exception {};


	/// ファイルに保存する形式。GDI+ でサポートされる形式のみ。
	struct FileFormat {
		enum _enum {
			bmp , /// ビットマップ画像ファイル形式。
			gif , /// GIF画像ファイル形式。
			jpeg, /// jpeg画像ファイル形式。
			png , /// PNG画像ファイル形式。
			tiff, /// TIFF画像ファイル形式。
		};
		BALOR_NAMED_ENUM_MEMBERS(FileFormat);
	};


	/// ビットマップの形式。
	struct Format {
		Format();
		/// １ピクセルあたりのビット数と RGB のビットマスクから作成。
		explicit Format(int bitsPerPixel, int rMask = 0, int gMask = 0, int bMask = 0, int aMask = 0);

		/// モノクロパレットビットマップ。
		static const Format palette1bpp;
		/// １６色パレットビットマップ。
		static const Format palette4bpp;
		/// ２５６色パレットビットマップ。
		static const Format palette8bpp;
		/// Format(16, 0x7C00, 0x03E0, 0x001F) である１６ビット画像。
		static const Format rgb16bpp;
		/// Format(16, 0xF800, 0x07E0, 0x001F) である１６ビット画像。
		static const Format rgb16bpp565;
		/// バイト配列の並びが B, G, R である２４ビット画像。
		static const Format rgb24bpp;
		/// Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000) である３２ビット画像。
		static const Format argb32bpp;

		bool operator==(const Format& value) const;
		bool operator!=(const Format& value) const;

		/// １ピクセルあたりのビット数。
		int bitsPerPixel;
		/// １ピクセルで赤の占めるビットマスク。bitsPerPixel が 16 または 32 の場合のみ有効、それ以外では 0。
		int rMask;
		/// １ピクセルで緑の占めるビットマスク。bitsPerPixel が 16 または 32 の場合のみ有効、それ以外では 0。
		int gMask;
		/// １ピクセルで青の占めるビットマスク。bitsPerPixel が 16 または 32 の場合のみ有効、それ以外では 0。
		int bMask;
		/// １ピクセルでアルファチャンネルの占めるビットマスク。bitsPerPixel が 32 の場合のみ有効、それ以外では 0。
		int aMask;
	};


public:
	/// ヌルハンドルで作成。
	Bitmap();
	Bitmap(Bitmap&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Bitmap(HBITMAP handle, bool owned = false);
	/// ストリームから作成。
	explicit Bitmap(Stream& stream, bool bottomUp = true, bool useIcm = false);
	explicit Bitmap(Stream&& stream, bool bottomUp = true, bool useIcm = false);
	/// ファイルから読み込んで作成。
	explicit Bitmap(StringRange filePath, bool bottomUp = true, bool useIcm = false);
	/// ビットマップの大きさ、フォーマット変更して作成。
	Bitmap(HBITMAP bitmap, const Size& size, Bitmap::Format format, bool bottomUp = true);
	Bitmap(HBITMAP bitmap, int width, int height, Bitmap::Format format, bool bottomUp = true);
	/// 大きさとフォーマット、ピクセルのメモリデータから作成。srcStride は srcPixels の１ラインあたりのバイト数。
	explicit Bitmap(const Size& size, Bitmap::Format format = Format::argb32bpp, bool bottomUp = true, const void* srcPixels = nullptr, int srcStride = 0, bool srcBottomUp = true);
	Bitmap(int width, int height, Bitmap::Format format = Format::argb32bpp, bool bottomUp = true, const void* srcPixels = nullptr, int srcStride = 0, bool srcBottomUp = true);
	~Bitmap();
	Bitmap& operator=(Bitmap&& value);

public:
	/// １ピクセルあたりのビット数。
	int bitsPerPixel() const;
	/// ピクセルデータが左下から始まるかどうか。DDB の場合は常に true を返す。
	bool bottomUp() const;
	/// DIB ならば DIB、DDB ならば DDB のまま複製して返す。
	Bitmap clone() const;
	static Bitmap clone(HBITMAP handle);
	/// DDB ビットマップを作成する
	static Bitmap createDDB(const Size& size);
	static Bitmap createDDB(int width, int height);
	/// フォーマットとピクセルデータが一致するかどうかを返す。フォーマットがたまたま同じであったとしても、DDB と DIB の比較は常にfalseを返す。
	bool equalsBits(const Bitmap& rhs, bool exceptAlpha = false) const;
	static bool equalsBits(const Bitmap& lhs, const Bitmap& rhs, bool exceptAlpha = false);
	/// ビットマップの形式。アルファマスクを取得する手段が無いので DIB の３２ビット画像は全てアルファマスク付きとみなすことに注意。
	Bitmap::Format format() const;
	/// 画像の高さ。
	int height() const;
	/// DDB かどうか。
	bool isDDB() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// パレット。パレット形式のビットマップでなければ操作できない。
	std::vector<Color, std::allocator<Color> > palette() const;
	void palette(ArrayRange<const Color> value);
	/// ピクセルデータへのポインタ。メモリ配置は bottomUp 関数で、１ラインのバイト数は stride 関数で調べられる。isDDB() が true の場合は nullptr を返す。
	/// GDI の非同期描画との衝突を避けるためにメモリにアクセスする前に Graphics::flush 関数を実行したほうがよい。
	unsigned char* pixels();
	const unsigned char* pixels() const;
	/// アルファチャンネルを全てのピクセルに乗算する。DIB の 32ビット画像の場合のみ有効。
	/// Graphics::blend 関数でアルファチャンネルを使う場合はあらかじめこの関数を呼んでおく必要がある。
	void premultiplyAlpha();
	/// ストリームに保存する。quality は jpeg 形式で保存する場合の品質で 0 ～ 100 で設定する。
	void save(Stream& stream, Bitmap::FileFormat format, int quality = 75) const;
	void save(Stream&& stream, Bitmap::FileFormat format, int quality = 75) const;
	/// ファイルに保存する。ファイル形式は拡張子から判別する。判別できなかった場合は png 形式で保存される。quality は jpeg 形式の品質で 0 ～ 100 で設定する。
	void save(StringRange filePath, int quality = 75) const;
	/// ファイルに保存する。quality は jpeg 形式の品質で 0 ～ 100 で設定する。
	void save(StringRange filePath, Bitmap::FileFormat format, int quality = 75) const;
	/// 画像のサイズ。
	Size size() const;
	/// ピクセルデータの１ラインが何バイトか。
	int stride() const;
	/// DDB ビットマップとして複製する。
	Bitmap toDDB() const;
	static Bitmap toDDB(HBITMAP handle);
	/// DIB ビットマップとして複製する。
	Bitmap toDIB(bool bottomUp = true) const;
	static Bitmap toDIB(HBITMAP handle, bool bottomUp = true);
	/// 画像の幅。
	int width() const;

public:
	/// HBITMAP への自動変換 ＆ null チェック用
	operator HBITMAP() const { return _handle; }

private:
	HBITMAP _handle;
	bool _owned;
};



	}
}