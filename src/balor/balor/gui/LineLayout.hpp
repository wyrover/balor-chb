#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {



/**
* 子コントロールを追加した順に左上から右（または左上から下）に並べていき、はみ出そうになったら改行するレイアウト。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"LineLayout Sample");

Label label(frame, 0, 0, 0, 0, L"ウインドウの大きさを変えるとコントロールの配置が変化する");
Button button0(frame, 0, 0, 0, 0, L"ボタン0");
Button button1(frame, 0, 0, 0, 0, L"ボタン1");
Button button2(frame, 0, 0, 0, 0, L"ボタン2");
Edit edit(frame, 0, 0, 0, 0, 100, 20);
CheckBox check0(frame, 0, 0, 0, 0, L"チェック0");
CheckBox check1(frame, 0, 0, 0, 0, L"チェック1");

LineLayout layout(frame);
layout.perform();
frame.onResized() = [&] (Frame::Resized& ) {
layout.perform();
};

frame.runMessageLoop();
* </code></pre>
*/
class LineLayout : private NonCopyable {
public:
	/// 何もしないレイアウトを作成。
	LineLayout();
	LineLayout(LineLayout&& value);
	/// レイアウト対象と縦配置かどうかから作成。
	LineLayout(Control& target, bool vertical = false);
	~LineLayout();
	LineLayout& operator=(LineLayout&& value);

public:
	/// 子コントロール同士の横と縦の隙間。初期値は Size(10, 20)。
	Size margin() const;
	void margin(const Size& value);
	void margin(int width, int height);
	/// レイアウトを更新する。
	void perform();
	/// Scaler を使ってレイアウト設定をスケーリングする。コントロールは別途スケーリングが必要。
	void scale(const Scaler& scaler);
	/// 最初の子コントロールをどこに配置するか。初期値は Size(10, 10)。
	Point startPosition() const;
	void startPosition(const Point& value);
	void startPosition(int x, int y);
	/// レイアウト対象のコントロール。
	Control* target();
	/// 縦配置かどうか。
	bool vertical() const;
	void vertical(bool value);

private:
	Reference<Control> _target;
	bool _vertical;
	Point _startPosition;
	Size _margin;
};



	}
}