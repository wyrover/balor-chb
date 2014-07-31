#pragma once

#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/Event.hpp>
#include <balor/Listener.hpp>
#include <balor/Point.hpp>
#include <balor/Size.hpp>

struct HWND__;


namespace balor {
	namespace gui {



/**
* フォント選択ダイアログ。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"FontDialog sample");

FontDialog dialog;
dialog.font(Font::defaultGui());
Button selectFont(frame, 20, 10, 0, 0, L"フォント選択", [&] (Button::Click& ) {
if (dialog.show(frame)) {
frame.invalidate();
}
});
frame.onPaint() = [&] (Frame::Paint& e) {
auto g = e.graphics();
g.brush(Color::control());
g.clear();
g.font(dialog.font());
g.textColor(dialog.color());
g.backTransparent(true);
g.drawText(L"aBcａＢｃあいうえお漢字", 10, 100);
};

frame.runMessageLoop();
* </code></pre>
*/
class FontDialog {
public:
	typedef ::HWND__* HWND;
	typedef ::balor::graphics::Color Color;
	typedef ::balor::graphics::Font Font;
	typedef ::balor::graphics::Font::HFONT HFONT;


	/// FontDialog のイベントの親クラス。
	struct Event : public balor::Event {
		Event(HWND sender);

		/// ダイアログのウインドウハンドル。
		HWND sender() const;

	private:
		HWND _sender;
	};


	/// ダイアログを初期化するイベント。
	struct Init : public Event {
		Init(HWND sender);

		/// ダイアログのスクリーン座標位置。
		/// 現状ではアプリケーションのプロセスが起動してから最初の一回しか変更できない模様。次回からは前回の位置が復元されている。
		Point position() const;
		void position(const Point& value);
		void position(int x, int y);
		/// ダイアログのクライアント領域の大きさ。
		Size clientSize() const;
		void clientSize(int width, int height);
		void clientSize(const Size& value);
	};


public:
	FontDialog();
	FontDialog(FontDialog&& value);
	virtual ~FontDialog();

	/// 文字セットを選択するＵＩを有効にするかどうか。初期値は true。
	bool charsetUI() const;
	void charsetUI(bool value);
	/// フォントの初期色または選択したフォントの色。初期値は Color::black()。
	Color color() const;
	void color(const Color& value);
	/// 取り消し線、下線、色を選択するＵＩを表示するかどうか。初期値は true。
	bool effectsUI() const;
	void effectsUI(bool value);
	/// 固定ピッチフォントのみ選択できるかどうか。初期値は false。
	bool fixedPitchOnly() const;
	void fixedPitchOnly(bool value);
	/// 初期フォントまたは選択したフォント。初期値はヌルハンドルの Font。
	const Font& font() const;
	void font(HFONT value);
	/// ダイアログを初期化するイベント。
	Listener<FontDialog::Init&>& onInit();
	/// 非 OEM、シンボル、ANSI 文字セットを選択できるかどうか。初期値は false。
	bool scriptsOnly() const;
	void scriptsOnly(bool value);
	/// ダイアログボックスを表示する。色を選択したかどうかを返す。
	bool show(HWND owner);
	/// 縦書きフォントを選択できるかどうか。初期値は true。
	bool verticalFonts() const;
	void verticalFonts(bool value);

protected:
	Font _font;
	Color _color;
	int _flags;
	Listener<FontDialog::Init&> _onInit;
};



	}
}