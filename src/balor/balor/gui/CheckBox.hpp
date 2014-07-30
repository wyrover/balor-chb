#pragma once

#include <balor/gui/Button.hpp>


namespace balor {
	namespace gui {



/**
* チェックボックス。
*
* オーナードローにするとチェック状態等を自分で管理しなければならなくなるので単に WM_PAINT を乗っ取る。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"CheckBox Sample");

auto onStateChange = [&] (CheckBox::StateChange& ) {
frame.invalidate();
};
CheckBox check0(frame, 20, 10, 0, 0, L"色を白に変える", onStateChange);
CheckBox check1(frame, 20, 40, 0, 0, L"形を四角に変える", onStateChange);

frame.onPaint() = [&] (Frame::Paint& e) {
auto g = e.graphics();
g.brush(Color::control());
g.clear();
g.brush(check0.checked() ? Color::white() : Color::yellow());
if (check1.checked()) {
g.drawRectangle(10, 80, 200, 200);
} else {
g.drawEllipse(10, 80, 200, 200);
}
};

frame.runMessageLoop();
* </code></pre>
*/
class CheckBox : public ButtonBase {
public:
	/// チェック状態。
	struct State {
		enum _enum {
			unchecked = 0x0000, /// チェックされていない。
			checked = 0x0001, /// チェックされている。
			indeterminate = 0x0002, /// 不確定状態。
		};
		BALOR_NAMED_ENUM_MEMBERS(State);
	};


	/// CheckBox のイベントの親クラス。
	typedef EventWithSubclassSender<CheckBox, Control::Event> Event;

	typedef Event StateChange;
	typedef PaintEvent<CheckBox, Event> Paint;


public:
	/// ヌルハンドルで作成。
	CheckBox();
	CheckBox(CheckBox&& value, bool checkSlicing = true);
	/// 親、位置、大きさ、文字列から作成。大きさを０にすると getPreferredSize 関数で求める。
	CheckBox(Control& parent, int x, int y, int width, int height, StringRange text, Listener<CheckBox::StateChange&> onStateChange = Listener<CheckBox::StateChange&>());
	virtual ~CheckBox();
	CheckBox& operator=(CheckBox&& value);

public:
	/// チェック状態を自動的に更新するかどうか。初期値は true。
	bool autoCheck() const;
	void autoCheck(bool value);
	/// チェックされているかどうか。
	bool checked() const;
	void checked(bool value);
	/// 現在の文字列やフォント等から最適な大きさを求める。multiline() が true の場合 width を固定すると最適な height が求まる。TODO:文字列に改行が含まれる場合にフォントを変更すると正確に求められない模様。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 描画するイベント。
	Listener<CheckBox::Paint&>& onPaint();
	/// チェック状態が変化した、あるいは autoCheck() が false の場合は変化が要求されたイベント。
	Listener<CheckBox::StateChange&>& onStateChange();
	/// トグルボタンのような外見を持つかどうか。初期値は false。
	bool pushLike() const;
	void pushLike(bool value);
	/// ボタンが文字列の右に表示されるかどうか。初期値は false。
	bool rightButton() const;
	void rightButton(bool value);
	/// チェック状態。
	CheckBox::State state() const;
	void state(CheckBox::State value);
	/// ３つのチェック状態を持つかどうか。false の場合は２つのチェック状態を持つ。初期値は false。
	bool threeState() const;
	void threeState(bool value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	bool _stateChanging;
	Listener<CheckBox::Paint&> _onPaint;
	Listener<CheckBox::StateChange&> _onStateChange;
};



	}
}