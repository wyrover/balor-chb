#pragma once

#include <balor/graphics/ImageList.hpp>
#include <balor/graphics/Region.hpp>
#include <balor/ArrayRange.hpp>
#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct _IMAGELIST;
struct HBITMAP__;
struct HBRUSH__;
struct HDC__;
struct HFONT__;
struct HICON__;
struct HPEN__;
struct HRGN__;
struct HWND__;

namespace balor {
class Point;
class Rectangle;
class Size;
}


namespace balor {
	namespace graphics {

class Brush;
class Color;
class Font;
class GraphicsPath;
class Pen;



/**
 * GDI による描画処理を扱う。
 * 
 * 図形を塗りつぶしせずに輪郭だけ描きたい場合は brush() に Brush::hollow() を設定する。
 * 図形を塗りつぶしだけしたい場合は pen() に Pen::hollow() を設定する。この場合図形の右下部分は描画されない。
 * drawText 関数を使う場合は backTransparent() に注意する。
 * ビットマップを描画するには Bitmap から Graphics を作成して copy 関数を使う。
 * 拡大縮小を伴って copy 関数を使う場合は copyMode() に注意する。
 * 半透明描画や、アルファチャンネルによる半透明合成をするには Bitmap::premultiplyAlpha 関数と blend 関数を使う。
 * カラーキーによる色抜きをするには ImageList と drawImageList 関数を使う
 * 設定を変更する関数では旧設定を返すようになっている。設定をまとめて保存、復元するには state 関数を使う。
 */
class Graphics : private NonCopyable {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::HBITMAP__* HBITMAP;
	typedef ::HBRUSH__* HBRUSH;
	typedef ::HDC__* HDC;
	typedef ::HFONT__* HFONT;
	typedef ::HICON__* HICON;
	typedef ::HPEN__* HPEN;
	typedef ::HRGN__* HRGN;
	typedef ::HWND__* HWND;


	/// コピー処理モード。
	struct CopyMode {
		enum _enum {
			andScans    = 1, /// 縮小で消えるピクセルを残されるピクセルに AND 演算する。
			orScans     = 2, /// 縮小で消えるピクセルを残されるピクセルに OR 演算する。
			deleteScans = 3, /// 縮小で消えるピクセルをそのまま消す。
			halftone    = 4, /// 縮小で消えるピクセルを残されるピクセルと混色する。
		};
		BALOR_NAMED_ENUM_MEMBERS(CopyMode);
	};

	/// コピーの演算方法。組み合わせで指定する。
	struct CopyOperation {
		enum _enum {
			captureBlt        = 0x40000000, /// コピー元が半透明なウインドウであってもピクセルをコピーする。
			destinationInvert = 0x00550009, /// コピー先の色を反転する。
			mergeCopy         = 0x00C000CA, /// コピー元とコピー先のブラシを AND 演算する。
			mergePaint        = 0x00BB0226, /// コピー元の色を反転してコピー先と OR 演算する。
			notSourceCopy     = 0x00330008, /// コピー元の色を反転してコピーする。
			notSourceErase    = 0x001100A6, /// コピー元とコピー先を OR 演算した後反転する。
			patCopy           = 0x00F00021, /// コピー先のブラシで塗る。
			patInvert         = 0x005A0049, /// コピー先のブラシとコピー先を XOR 演算する。
			patPaint          = 0x00FB0A09, /// コピー元を反転した色とコピー先のブラシを OR 演算し、結果とコピー先を OR 演算する。
			sourceAnd         = 0x008800C6, /// コピー元とコピー先を AND 演算する。
			sourceCopy        = 0x00CC0020, /// コピー元をコピーする。
			sourceErase       = 0x00440328, /// コピー元とコピー先を反転した色を AND 演算する。
			sourceInvert      = 0x00660046, /// コピー元とコピー先を XOR 演算する。
			sourcePaint       = 0x00EE0086, /// コピー元とコピー先を OR 演算する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(CopyOperation);
	};

	/// 文字列描画のフラグ。組み合わせで指定する。
	struct TextFormat {
		enum _enum {
			none                      = 0         , 
			bottom                    = 0x00000028, /// 矩形の下端に文字列を揃える。singleLine も含める。
			default                   = 0x00000810, /// noPrefix かつ wordBreak。
			editControl               = 0x00002000, /// 複数行エディットコントロールと同じように描画する。
			endEllipsis               = 0x00008000, /// 文末が矩形からはみ出す場合に「...」を表示する。
			expandTabs                = 0x00000040, /// タブを展開する。
			externalLeading           = 0x00000200, /// 行の高さに、外部レディングの高さを追加する。
			hidePrefix                = 0x00100000, /// L'&' によって下線を表示しない。
			horizontalCenter          = 0x00000001, /// 矩形の水平方向の中央に文字列を揃える。
			noClipping                = 0x00000100, /// 矩形からはみ出ても描画する。
			noFullWidthCharacterBreak = 0x00080000, /// 一つの文字を複数の文字コードの連続で表す時に途中で改行されないようにする。
			noPrefix                  = 0x00000800, /// L'&' を特殊文字とみなさないようにする。
			pathEllipsis              = 0x00004000, /// ファイルパスを表示し切れない場合に適切に短縮する。
			prefixOnly                = 0x00200000, /// L'&' によって表示される下線だけ描画する。
			right                     = 0x00000002, /// 矩形の右端に文字列を揃える。
			rightToLeft               = 0x00020000, /// フォントがヘブライ語かアラビア語の場合に右から左に文字を描画する。
			singleLine                = 0x00000020, /// 改行せずに描画する。
			verticalCenter            = 0x00000024, /// 矩形の垂直方向の中央に文字列を揃える。singleLine も含める。
			wordBreak                 = 0x00000010, /// 単語の途中で改行しないようにする。
			wordEllipsis              = 0x00040000, /// 矩形から単語の途中ではみ出す場合に「...」を表示する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(TextFormat);
	};


	/// ペンやブラシ等の設定を保存したオブジェクト。
	class State {
	public:
		State();
		State(HDC handle);

	public:
		HDC handle() const;
		int id() const;

	private:
		HDC _handle;
		int _id;
	};


public:
	/// ヌルハンドルで作成。
	Graphics();
	Graphics(Graphics&& value);
	/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
	explicit Graphics(HDC handle, bool owned = false);
	/// ビットマップのデバイスを作成。ビットマップを先に破棄しようとすると失敗するので、必ず Graphics のほうを先に破棄すること。
	explicit Graphics(HBITMAP bitmap);
	/// ウインドウハンドルから作成。
	explicit Graphics(HWND control);
	~Graphics();

	Graphics& operator=(Graphics&& value);

public:
	/// 背景色。
	Color backColor() const;
	Color backColor(Color value);
	/// テキスト、ハッチブラシ、ペンで描画する時に背景をブラシで塗らないかどうか。
	bool backTransparent() const;
	bool backTransparent(bool value) const;
	/// 半透明合成を行う。alpha に不透明度０～１を指定する。copyMode() の影響は受けず、常に Graphics::CopyMode::deleteScans と同じ挙動になる。
	/// src がアルファチャンネルを持った 32ビットのビットマップ（DDB でも良い）である場合に限り、srcAlphaChannel 引数を true にするとアルファチャンネルによる半透明合成を行う。
	/// この場合 src の画像は Bitmap::premultiplyAlpha 関数であらかじめアルファを乗算しておく必要がある。また alpha 引数も同時に指定できる。
	/// copy 関数のような反転コピーはサポートされず、ソースの範囲外をコピーしようとするとエラーになる。
	void blend(const Point& dstPos, HDC src, float alpha, bool srcAlphaChannel = false);
	void blend(int dstX, int dstY, HDC src, float alpha, bool srcAlphaChannel = false);
	void blend(const Point& dstPos, HDC src, const Point& srcPos, const Size& size, float alpha, bool srcAlphaChannel = false);
	void blend(int dstX, int dstY, HDC src, int srcX, int srcY, int width, int height, float alpha, bool srcAlphaChannel = false);
	void blend(const Rectangle& dstRect, HDC src, float alpha, bool srcAlphaChannel = false);
	void blend(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, float alpha, bool srcAlphaChannel = false);
	void blend(const Rectangle& dstRect, HDC src, const Rectangle& srcRect, float alpha, bool srcAlphaChannel = false);
	void blend(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, int srcX, int srcY, int srcWidth, int srcHeight, float alpha, bool srcAlphaChannel = false);
	/// 図形を塗りつぶすブラシ。色を直接設定できる。塗らない場合は Brush::hollow() を設定する。
	Brush brush() const;
	Brush brush(HBRUSH value);
	Brush brush(Color value);
	/// ブラシの原点。copyMode を Graphics::CopyMode::halftone に設定した場合は再設定する必要がある。
	Point brushOrigin() const;
	Point brushOrigin(const Point& value);
	/// 現在のブラシで全体を塗りつぶす。
	void clear();
	/// クリップ領域。
	Region clip() const;;
	void clip(HRGN region);
	/// クリップ領域を合成する。
	void combineClip(HRGN region, Region::Operation operation);
	void combineClip(const Rectangle rect, Region::Operation operation);
	/// ピクセルのコピーを行う。大きさに負数を指定すると反転コピーになる。
	/// 縮小や拡大コピーする場合、copyMode を Graphics::CopyMode::halftone にしておかないととても汚くなる。ただし Graphics::CopyMode::halftone だとアルファチャンネル情報がおかしくなる。
	/// 綺麗にする必要が無くても少なくとも Graphics::CopyMode::deleteScans にしておかないと思ったような結果にはならない。
	void copy(const Point& dstPos, HDC src, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(int dstX, int dstY, HDC src, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(const Point& dstPos, HDC src, const Point& srcPos, const Size& size, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(int dstX, int dstY, HDC src, int srcX, int srcY, int width, int height, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(const Rectangle& dstRect, HDC src, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(const Rectangle& dstRect, HDC src, const Rectangle& srcRect, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	void copy(int dstX, int dstY, int dstWidth, int dstHeight, HDC src, int srcX, int srcY, int srcWidth, int srcHeight, Graphics::CopyOperation operation = CopyOperation::sourceCopy);
	/// コピー処理のモード。Graphics::CopyMode::halftone に設定した場合は brushOrigin を再設定する必要がある。
	Graphics::CopyMode copyMode() const;
	Graphics::CopyMode copyMode(Graphics::CopyMode value);
	/// ｘ軸のドット密度。
	int dpiX() const;
	/// ｙ軸のドット密度。
	int dpiY() const;
	/// 円弧を描く。角度は 0 ～ 360 で反時計周り。
	void drawArc(const Rectangle& rect, float startAngle, float sweepAngle);
	void drawArc(int x, int y, int width, int height, float startAngle, float sweepAngle);
	/// 点 p0 から 点 p3 にベジェ曲線を描く。
	void drawBezier(const Point& p0, const Point& p1, const Point& p2, const Point& p3);
	void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
	/// 連続したベジェ曲線を描く。４点指定で連続する始点と終点を共通化した点の配列で、３で割ると１余る長さでなければならない。
	void drawBeziers(ArrayRange<const Point> points);
	/// 円を描く。
	void drawCircle(const Point& point, int radius);
	void drawCircle(int x, int y, int radius);
	/// 楕円を描く。
	void drawEllipse(const Rectangle& rect);
	void drawEllipse(int x, int y, int width, int height);
	/// アイコンを描く。masked を false にすると背景を透過しない。
	void drawIcon(HICON icon, const Point& point, bool masked = true);
	void drawIcon(HICON icon, int x, int y, bool masked = true);
	void drawIcon(HICON icon, const Rectangle& rect, bool masked = true);
	void drawIcon(HICON icon, int x, int y, int width, int height, bool masked = true);
	/// イメージリストの中の画像を描く。大きさを０にすると原寸で描画する。alpha が 1 未満の場合は半透明合成する。ただしグレイスケールと同時には有効にならない。
	void drawImageList(HIMAGELIST imageList, int index, const Rectangle& rect, ImageList::State state = ImageList::State::normal, float alpha = 1.0f, bool grayscale = false);
	void drawImageList(HIMAGELIST imageList, int index, int x, int y, int width = 0, int height = 0, ImageList::State state = ImageList::State::normal, float alpha = 1.0f, bool grayscale = false);
	/// 線分を描く。終点のピクセルは描かれない。
	void drawLine(const Point& p0, const Point& p1);
	void drawLine(int x0, int y0, int x1, int y1);
	/// 線分を連続で描く。
	void drawLines(ArrayRange<const Point> points);
	/// GraphicsPath で描かれた線を描く。ペンで線のみ描く。ブラシで塗る場合は fillPath 関数を使う。
	void drawPath(GraphicsPath& path);
	/// 扇型を描く。角度は 0 ～ 360 で反時計周り。
	void drawPie(const Rectangle& rect, float startAngle, float sweepAngle);
	void drawPie(int x, int y, int width, int height, float startAngle, float sweepAngle);
	/// 多角形を描く。塗りつぶし方法は fillAlternate 関数で設定する。
	void drawPolygon(ArrayRange<const Point> points);
	/// 長方形を描く。
	void drawRectangle(const Rectangle& rect);
	void drawRectangle(int x, int y, int width, int height);
	/// 長方形を連続で描く。
	void drawRectangles(ArrayRange<const Rectangle> rects);
	/// 領域の輪郭を描く。やや特殊だが輪郭描画に使うブラシを引数で指定する。
	void drawRegion(HRGN region, HBRUSH brush, int lineWidth, int lineHeight);
	/// 角の丸い長方形を描く。
	void drawRoundRectangle(const Rectangle& rect, const Size ellipseSize);
	void drawRoundRectangle(int x, int y, int width, int height, int ellipseWidth, int ellipseHeight);
	/// 文字列を書く。文字の背景を塗らないようにするには backTransparent を true に設定する。
	void drawText(StringRange s, const Point& point);
	void drawText(StringRange s, int x, int y);
	/// 矩形の範囲内で文字列を書く。escapement 及び orientation が 0 のフォントしかサポートしない。（＝縦書きは不可）
	void drawText(StringRange s, const Rectangle& rect, Graphics::TextFormat format = TextFormat::default, int tabWidth = 8);
	void drawText(StringRange s, int x, int y, int width, int height, Graphics::TextFormat format = TextFormat::default, int tabWidth = 8);
	/// drawPolygon 関数で交差する多角形を交互に塗りつぶすかどうか。false の場合は全て塗りつぶす。
	bool fillAlternate() const;
	bool fillAlternate(bool value);
	/// GraphicsPath で描かれた領域をブラシで塗る。
	void fillPath(GraphicsPath& path);
	/// 領域をブラシで塗る。
	void fillRegion(HRGN region);
	/// 境界色に囲まれた領域の塗りつぶしを行う。デバイスが更新領域でクリッピングされていると思ったような結果にならない事に注意。
	void floodFillBorder(const Point& point, const Color& borderColor);
	void floodFillBorder(int x, int y, const Color& borderColor);
	/// 指定した表面色の領域の塗りつぶしを行う。デバイスが更新領域でクリッピングされていると思ったような結果にならない事に注意。
	void floodFillSurface(const Point& point, const Color& surfaceColor);
	void floodFillSurface(int x, int y, const Color& surfaceColor);
	/// ハードが描画を終えるまで待つ。
	static void flush();
	/// 文字を描画するフォント。
	Font font() const;
	Font font(HFONT value);
	/// ウインドウの非クライアント領域も含めた Graphics を作成。
	static Graphics fromControlWithNonClientArea(HWND control);
	/// 指定した graphics と互換性のあるメモリデバイスを作成。graphics に nullptr を指定するとディスプレイと互換性のあるメモリデバイスを作成する。
	static Graphics fromGraphics(HDC graphics);
	/// ディスプレイを表す Graphics を作成。
	static Graphics fromScreen();
	/// このデバイスで指定した色に最も近い色を返す。
	Color getNearestColor(const Color& color) const;
	/// ピクセルの色を取得する。画像がアルファチャンネルを持っていたとしてもアルファ情報は取得できない。
	Color getPixel(const Point& point) const;
	Color getPixel(int x, int y) const;
	/// 点が描画できるかどうか。
	bool isVisible(const Point& point) const;
	bool isVisible(int x, int y) const;
	/// 領域が一部でも描画できるかどうか。
	bool isVisible(const Rectangle& rect) const;
	bool isVisible(int x, int y, int width, int height) const;
	/// 文字列の描画サイズを求める。普通の横描きフォントのみ有効。
	Size measureText(StringRange s);
	/// 指定した幅で表示できる文字までで切り捨てた場合の描画サイズを求める。普通の横描きフォントのみ有効。
	Size measureText(StringRange s, int width);
	/// 文字列の描画サイズを求める。普通の横描きフォントのみ有効。
	Size measureText(StringRange s, const Size& layoutArea, Graphics::TextFormat format = TextFormat::default, int tabWidth = 8);
	Size measureText(StringRange s, int width, int height, Graphics::TextFormat format = TextFormat::default, int tabWidth = 8);
	/// ペンのマイター接合の長さ。
	float miterLimit() const;
	float miterLimit(float value);
	/// デストラクタでハンドルを破棄するかどうか。変更は要注意。
	bool owned() const;
	void owned(bool value);
	/// 図形の輪郭を描くペン。色を直接設定できる。輪郭を描かない場合は Pen::hollow() を設定する。
	Pen pen() const;
	Pen pen(HPEN value);
	Pen pen(Color value);
	/// dpiY() を元にピクセル→ポイント変換を行う。
	float pixelToPoint(int pixel);
	/// dpiY() を元にポイント→ピクセル変換を行う。
	int pointToPixel(float point);
	/// ピクセルの色を設定する。
	void setPixel(const Point& point, const Color& color);
	void setPixel(int x, int y, const Color& color);
	/// 現在の設定の保存と復元。
	Graphics::State state() const;
	void state(Graphics::State value);
	/// 文字列の文字間隔。一行描画の drawText 関数でプロポーショナルではないフォントを使った場合のみ有効。
	/// プロポーショナルフォントで描画する場合にこの設定が０（規定値）以外だとうまく描画されないので注意。
	int textCharExtra() const;
	int textCharExtra(int value);
	/// テキストの色。
	Color textColor() const;
	Color textColor(Color value);
	/// クリップ領域を平行移動する。
	void translateClip(int dx, int dy);
	/// 実際に描画可能な領域を含む矩形。画像から作成している場合は画像の大きさ。
	Rectangle visibleClipBounds() const;

public:
	/// HDC への自動変換 ＆ null チェック用
	operator HDC() const { return _handle; }

private:
	HDC _handle;
	bool _owned;
};



	}
}