#pragma once

#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct HFONT__;
struct tagLOGFONTW;

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
class String;
}


namespace balor {
	namespace graphics {



/**
 * GDI フォントを表す。
 * 
 * フォントサイズにポイント単位が使いたい場合は Graphics::pointToPixel 関数を使う。
 * 縦書き指定は コンストラクタでフォント名の先頭に@をつけ、escapement と orientation を 2700 に指定する。
 * 縦書き専用フォントでなくとも escapement で一応縦書きにはなるが「。」等の小さな文字が左下に寄せられるので使いにくい。
 * TODO:平均文字幅、アセント、ディセント･･等の情報取得。
 */
class Font : private NonCopyable {
public:
	typedef ::HFONT__* HFONT;
	typedef ::tagLOGFONTW LOGFONTW;

	/// フォントの描画品質。
	/// XPでは nonantialiased 以外は antialiased と等しいようだ。
	/// Vistaでは antialiased と nonantialiased はＸＰと同じだがそれ以外は ClearType のぼかしが見られる。
	/// Vistaにおいて、ＭＳ ゴシックはフォントのピクセルサイズがアンチエイリアスは 25 未満、 ClearType のぼかしは 23 未満だとかからなくなるが、
	/// メイリオは 23 未満でも両方かかる。
	struct Quality {
		enum _enum {
			default          = 0, /// 品質を重視しない。
			draft            = 1, /// proof の次に品質を重視する。
			proof            = 2, /// 最も品質を重視する。
			nonantialiased   = 3, /// アンチエイリアスをかけない。
			antialiased      = 4, /// アンチエイリアスをかける。
			clearType        = 5, /// クリアタイプのアンチエイリアスをかける。
		};
		BALOR_NAMED_ENUM_MEMBERS(Quality);
	};

	/// フォントのスタイル。組み合わせで指定する。
	struct Style {
		enum _enum {
			regular   = 0x00, /// 普通の字体。
			bold      = 0x01, /// 太字。
			italic    = 0x02, /// 斜体。
			strikeout = 0x04, /// 取り消し線付き。
			underline = 0x08, /// 下線付き。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Style);
	};

public:
	/// ヌルハンドルで作成。
	Font();
	Font(Font&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Font(HFONT handle, bool owned = false);
	/// フォントを新しい大きさとスタイルで複製する。
	Font(const Font& font, int height, Font::Style style = Style::regular);
	/// 各引数を元にフォントを作成する。height はピクセルサイズ。
	Font(StringRange name, int height, Font::Style style = Style::regular, Font::Quality quality = Quality::proof, int escapement = 0, int orientation = 0);
	~Font();
	Font& operator=(Font&& value);

public:
	/// 文字の平均幅。
	int averageWidth() const;
	/// 複製を作成して返す。
	Font clone() const;
	static Font clone(HFONT handle);
	/// 文字送り方向の角度。（0～3600）
	int escapement() const;
	/// LOGFONTW 構造体から作成。
	static Font fromLogFont(const LOGFONTW& logfont);
	/// 文字のピクセル単位の高さ。
	int height() const;
	/// フォント名。コンストラクタで指定した名前とは限らない。最終的に選ばれた名前になる。
	String name() const;
	/// 使用できるフォント名リスト。
	static std::vector<String, std::allocator<String> > names();
	/// 文字のベースラインに対する表示方向の角度。（0～3600）
	int orientation() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// 描画品質
	Font::Quality quality() const;
	/// フォントスタイル。
	Font::Style style() const;

public:// システムフォント一覧。
	static Font ansiFixed();
	static Font ansiVar();
	static Font deviceDefault();
	/// Control のデフォルトフォント。
	static Font defaultGui();
	static Font oemFixed();
	static Font system();
	static Font systemFixed();

public:
	/// HFONT への自動変換 ＆ null チェック用
	operator HFONT() const { return _handle; }

private:
	HFONT _handle;
	bool _owned;
};



	}
}