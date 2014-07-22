#pragma once

#include <balor/gui/Button.hpp>


namespace balor {
	namespace gui {



/**
 * ラジオボタン。
 * 
 * オーナードローにするとチェック状態等を自分で管理しなければならなくなるので単に WM_PAINT を乗っ取る。
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
class RadioButton : public ButtonBase {
public:
	/// RadioButton のイベントの親クラス。
	typedef EventWithSubclassSender<RadioButton, Control::Event> Event;

	typedef Event Check;
	typedef PaintEvent<RadioButton, Event> Paint;


public:
	/// ヌルハンドルで作成。
	RadioButton();
	RadioButton(RadioButton&& value, bool checkSlicing = true);
	/// 親、位置、大きさ、文字列、firstOfGroup から作成。大きさを０にすると getPreferredSize 関数で求める。
	RadioButton(Control& parent, int x, int y, int width, int height, StringRange text, bool firstOfGroup = false);
	/// 親、位置、大きさ、文字列、onCheck イベント、firstOfGroup から作成。大きさを０にすると getPreferredSize 関数で求める。
	RadioButton(Control& parent, int x, int y, int width, int height, StringRange text, Listener<RadioButton::Check&> onCheck, bool firstOfGroup = false);
	virtual ~RadioButton();
	RadioButton& operator=(RadioButton&& value);

public:
	/// チェック状態を自動的に更新するかどうか。初期値は true。
	bool autoCheck() const;
	void autoCheck(bool value);
	/// チェックされているかどうか。
	bool checked() const;
	void checked(bool value);
	/// 現在の文字列やフォント等から最適な大きさを求める。multiline() が true の場合 width を固定すると最適な height が求まる。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// チェック状態が変化した、あるいは autoCheck() が false の場合は変化が要求されたイベント。
	Listener<RadioButton::Check&>& onCheck();
	/// 描画するイベント。
	Listener<RadioButton::Paint&>& onPaint();
	/// トグルボタンのような外見を持つかどうか。初期値は false。
	bool pushLike() const;
	void pushLike(bool value);
	/// ボタンが文字列の右に表示されるかどうか。初期値は false。
	bool rightButton() const;
	void rightButton(bool value);
	/// ラジオボタングループの開始コントロールかどうか。
	/// グループ内の一つをクリックするとそれまで同じグループ内でチェックされていたものからチェック状態が移る。
	/// 他のラジオボタングループが祖先にも兄弟にも子孫にも無く孤立しているなら設定する必要は無い。
	/// そうではない場合はグループ内で最初に追加した（zOrder() が最も高い）ラジオボタンを開始コントロールに設定する。
	bool firstOfGroup() const;
	void firstOfGroup(bool value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	bool _checkedChanging;
	Listener<RadioButton::Check&> _onCheck;
	Listener<RadioButton::Paint&> _onPaint;
};



	}
}