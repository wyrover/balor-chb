#pragma once

#include <balor/graphics/Color.hpp>
#include <balor/Event.hpp>
#include <balor/Listener.hpp>
#include <balor/Point.hpp>
#include <balor/Size.hpp>

struct HWND__;


namespace balor {
	namespace gui {



/**
 * 色選択ダイアログ。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"ColorDialog sample");

	Brush frameBrush;
	ColorDialog dialog;
	Button selectColor(frame, 20, 10, 0, 0, L"背景色選択", [&] (Button::Click& ) {
		if (dialog.show(frame)) {
			frameBrush = Brush(dialog.color());
			frame.brush(frameBrush);
		}
	});

	frame.runMessageLoop();
 * </code></pre>
 */
class ColorDialog {
public:
	typedef ::HWND__* HWND;
	typedef ::balor::graphics::Color Color;


	/// ColorDialog のイベントの親クラス。
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
	ColorDialog();
	ColorDialog(ColorDialog&& value);
	~ColorDialog();

	/// 初期色または作成した色。初期値は Color::black()。
	Color color() const;
	void color(const Color& value);
	/// 色作成ＵＩを表示するかどうか。false の場合でも色作成ボタンを押せば表示される。初期値は true。
	bool createColorUI() const;
	void createColorUI(bool value);
	/// １６色の作成済みの色から一つ取得する。初期値は全て Color::black()。
	Color getCustomColor(int index) const;
	/// ダイアログを初期化するイベント。
	Listener<ColorDialog::Init&>& onInit();
	/// １６色の作成済みの色を一つ設定する。
	void setCustomColor(int index, const Color& value);
	/// ダイアログボックスを表示する。色を選択したかどうかを返す。
	bool show(HWND owner);

protected:
	Color _color;
	Color _customColors[16];
	int _flags;
	Listener<ColorDialog::Init&> _onInit;
};



	}
}