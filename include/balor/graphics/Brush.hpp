#pragma once

#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>

struct HBITMAP__;
struct HBRUSH__;


namespace balor {
	namespace graphics {

class Bitmap;
class Color;


/**
 * GDI ブラシを表す。
 */
class Brush : private NonCopyable {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::HBRUSH__* HBRUSH;

	/// ハッチブラシの種類。
	struct Hatch {
		enum _enum {
			none             = -1, /// 単色ブラシ
			horizontal       =  0, /// 横縞模様。
			vertical         =  1, /// 縦縞模様。
			forwardDiagonal  =  2, /// ４５度右上がり縞模様。
			backwardDiagonal =  3, /// ４５度右下がり縞模様。
			cross            =  4, /// 縦横縞模様。
			diagonalCross    =  5, /// ４５度右上がり右下がり交差縞模様。
		};
		BALOR_NAMED_ENUM_MEMBERS(Hatch);
	};

public:
	/// ヌルハンドルで作成。
	Brush();
	Brush(Brush&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Brush(HBRUSH handle, bool owned = false);
	/// ビットマップからパターンブラシを作成する。作成後、ビットマップハンドルは参照されない。
	explicit Brush(HBITMAP bitmap);
	/// 色とハッチの種類から単色ブラシまたはハッチブラシを作成する。
	explicit Brush(const Color& color, Brush::Hatch hatch = Hatch::none);
	~Brush();

	Brush& operator=(Brush&& value);

public:
	/// パターンブラシのビットマップを返す。パターンブラシでない場合はヌルハンドルのビットマップが返る。
	Bitmap bitmap() const;
	/// 複製して返す。
	Brush clone() const;
	static Brush clone(HBRUSH handle);
	/// 単色またはハッチブラシの色。パターンブラシの場合は無意味。
	Color color() const;
	/// ハッチブラシの種類を返す。
	Brush::Hatch hatch() const;
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);

public: // システムブラシ一覧
	/// 白いブラシ。
	static Brush white();
	/// 明るい灰色ブラシ。
	static Brush lightGray();
	/// 暗い灰色ブラシ。
	static Brush darkGray();
	/// 黒いブラシ。
	static Brush black();
	/// 描画しないブラシ。
	static Brush hollow();

	static Brush activeBorder();
	static Brush activeCaption();
	static Brush activeCaptionText();
	static Brush appWorkspace();
	static Brush control();
	static Brush controlShadow();
	static Brush controlDarkShadow();
	static Brush controlLight();
	static Brush controlHighlight();
	static Brush controlText();
	static Brush desktop();
	static Brush gradientActiveCaption();
	static Brush gradientInactiveCaption();
	static Brush grayText();
	static Brush hotTrack();
	static Brush inactiveBorder();
	static Brush inactiveCaption();
	static Brush inactiveCaptionText();
	static Brush menu();
	static Brush menuBar();
	static Brush menuHighlight();
	static Brush menuText();
	static Brush scrollbar();
	static Brush selectedControl();
	static Brush selectedControlText();
	static Brush toolTip();
	static Brush toolTipText();
	static Brush window();
	static Brush windowFrame();
	static Brush windowText();

public:
	/// HBRUSH への自動変換 ＆ null チェック用
	operator HBRUSH() const { return _handle; }

private:
	HBRUSH _handle;
	bool _owned;
};



	}
}