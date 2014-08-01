#pragma once

#include <balor/Enum.hpp>

struct HICON__;

namespace balor {
	namespace graphics {
		class Cursor;
	}
class Point;
class Rectangle;
class Size;
}


namespace balor {
	namespace gui {


/**
* マウスに関する情報や操作を扱う。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"MessageBox Sample");

Label position(frame, 20, 10, 200, 0, L"現在のカーソル位置：");
frame.onMouseMove() = [&] (Frame::MouseMove& ) {
position.text(String(L"現在のカーソル位置：") + Mouse::position().x + L", " + Mouse::position().y);
};
Label change(frame, 20, 50, 200, 100, L"カーソルの変更");
change.edge(Label::Edge::client);
change.onCursorChange() = [&] (Label::CursorChange& e) {
e.cursor(Cursor::hand());
};
Label hide(frame, 20, 180, 200, 100, L"カーソルを非表示");
hide.edge(Label::Edge::client);
hide.onMouseEnter() = [&] (Label::MouseEnter& ) {
Mouse::hide();
};
hide.onMouseLeave() = [&] (Label::MouseLeave& ) {
Mouse::show();
};

frame.runMessageLoop();
* </code></pre>
*/
class Mouse {
public:
	typedef ::HICON__* HCURSOR;
	typedef ::balor::graphics::Cursor Cursor;

	/// マウスのボタンの種類。
	enum _enum {
		none = 0x00, /// どのボタンでもない。
		lButton = 0x01, /// 左ボタン。
		mButton = 0x04, /// 中央ボタン。
		rButton = 0x02, /// 右ボタン。
		xButton1 = 0x05, /// X1ボタン。
		xButton2 = 0x06, /// X2ボタン。
	};
	BALOR_NAMED_ENUM_MEMBERS(Mouse);

public:
	/// ボタンの数。
	static int buttonCount();
	/// 左右のボタン機能を交換しているかどうか。
	static bool buttonSwapped();
	/// マウスカーソルが移動可能なスクリーン座標領域。
	static Rectangle clip();
	static void clip(const Rectangle& value);
	/// マウスカーソル。
	static Cursor cursor();
	static void cursor(HCURSOR value);
	/// 二回のクリックがダブルクリックとみなされるピクセル範囲。
	static Size doubleClickSize();
	/// ダブルクリックとみなされるクリック間隔（ミリ秒）。
	static int doubleClickTime();
	/// マウスカーソルを非表示にする。表示非表示はカウントされる。
	static void hide();
	/// マウスホバーでとどまっていなければならない範囲ピクセル。
	static Size hoverSize();
	/// マウスホバーのデフォルト待機時間（ミリ秒）。
	static int hoverTime();
	/// ボタンが押されているかどうか。ウインドウメッセージループで状態が更新される。
	static bool isPushed(Mouse button);
	/// ボタンが押されているかどうか。ウインドウメッセージループによらずに現在の状態を取得する。
	static bool isPushedAsync(Mouse button);
	/// マウスカーソルの現在のスクリーン座標。
	static Point position();
	static void position(const Point& value);
	/// 最後にメッセージ処理されたマウスカーソルのスクリーン座標。
	static Point positionAtLastMessage();
	/// マウスが接続されているかどうか。
	static bool present();
	/// マウスカーソルを表示する。表示非表示はカウントされる。
	static void show();
	/// マウスカーソルの移動速度。１（遅い）～２０（早い）を返す。
	static int speed();
	/// マウスホイールが接続されているかどうか。
	static bool wheelPresent();
	/// マウスホイールで一度にスクロールするピクセル数。
	static int wheelScrollDelta();
	/// マウスホイールで一度にスクロールする行数。
	static int wheelScrollLines();
};



	}
}