#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {



/**
* グループボックス。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"GroupBox Sample");

GroupBox group(frame, 20, 10, 0, 0, L"背景色");
Brush redBrush(Color::red());
Brush greenBrush(Color::green());
Brush blueBrush(Color::blue());
RadioButton red(group, 20, 20, 0, 0, L"赤い背景", [&] (RadioButton::Check& ) {
frame.brush(redBrush);
frame.invalidate(true);
});
RadioButton green(group, 20, 50, 0, 0, L"緑の背景", [&] (RadioButton::Check& ) {
frame.brush(greenBrush);
frame.invalidate(true);
});
RadioButton blue(group, 20, 80, 0, 0, L"青い背景", [&] (RadioButton::Check& ) {
frame.brush(blueBrush);
frame.invalidate(true);
});
group.resize(); // 子コントロールに大きさをあわせる。

frame.runMessageLoop();
* </code></pre>
*/
class GroupBox : public Control {
public:
	/// ヌルハンドルで作成。
	GroupBox();
	GroupBox(GroupBox&& value, bool checkSlicing = true);
	/// 親、位置、大きさ、文字列から作成。
	GroupBox(Control& parent, int x, int y, int width, int height, StringRange text);
	virtual ~GroupBox();
	GroupBox& operator=(GroupBox&& value);

public:
	/// 背景ブラシ。設定しなければ親コントロールのブラシを使用する。
	virtual Brush brush() const;
	using Control::brush;
	/// 背景ブラシの原点。
	virtual Point brushOrigin() const;
	using Control::brushOrigin;
	/// 仮想的なコントロールのクライアント座標と大きさ。
	virtual Rectangle displayRectangle() const;
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// 子コントロールが収まる最適な大きさを求める。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);
};



	}
}