#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {


/**
* Button, CheckBox, RadioButton の共通処理をまとめた基底クラス。
*/
class ButtonBase : public Control {
public:
	/// 文字列の配置。
	struct TextAlign {
		enum _enum {
			topLeft = 0x00000500L, /// 左上。
			topCenter = 0x00000700L, /// 上中央。
			topRight = 0x00000600L, /// 右上。
			middleLeft = 0x00000D00L, /// 中央左。
			middleCenter = 0x00000F00L, /// 中央。
			middleRight = 0x00000E00L, /// 中央右。
			bottomLeft = 0x00000900L, /// 左下。
			bottomCenter = 0x00000B00L, /// 下中央。
			bottomRight = 0x00000A00L, /// 右下。
		};
		BALOR_NAMED_ENUM_MEMBERS(TextAlign);
	};


protected:
	ButtonBase();
	ButtonBase(ButtonBase&& value);
	virtual ~ButtonBase() = 0;

public:
	/// 文字列がはみ出す場合や改行コードによって改行するかどうか。初期値は false。
	bool multiline() const;
	void multiline(bool value);
	/// ボタンを押した状態かどうか。
	bool pushed() const;
	/// 文字列の配置。初期値は Button は ButtonBase::TextAlign::middleCenter、CheckBox と RadioButton は middleLeft。
	ButtonBase::TextAlign textAlign() const;
	void textAlign(ButtonBase::TextAlign value);
};



/**
* ボタンコントロール。
*
* チェックボックス、ラジオボタン、グループボックスはそれぞれ CheckBox, RadioButton, GroupBox クラスで実装されている。
* BS_BITMAP や BS_ICON スタイルは XP でビジュアルスタイルが強制的に無効になるのでサポートしない。
* カスタムドローや BCM_SETIMAGELIST は強制的にフェードアニメーションさせられるのでサポートしない。onPaint イベントで対応する。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"Button Sample");

Button button(frame, 20, 10, 0, 0, L"button", [&] (Button::Click& ) {
MsgBox::show(L"clicked");
});

frame.runMessageLoop();
* </code></pre>
*/
class Button : public ButtonBase {
public:
	/// Button のイベントの親クラス。
	typedef EventWithSubclassSender<Button, Control::Event> Event;

	typedef Event Click;
	typedef PaintEvent<Button, Event> Paint;


public:
	/// ヌルハンドルで作成。
	Button();
	Button(Button&& value, bool checkSlicing = true);
	/// 親、位置、大きさ、文字列から作成。大きさを０にすると getPreferredSize 関数で求める。
	Button(Control& parent, int x, int y, int width, int height, StringRange text, Listener<Button::Event&> onClick = Listener<Button::Event&>());
	virtual ~Button();
	Button& operator=(Button&& value);

public:
	/// Enter キーを押した時にフォーカスがなくとも押した事になるボタンの外見かどうか。初期値は false。
	bool defaultButton() const;
	void defaultButton(bool value);
	/// 現在の文字列やフォント等から最適な大きさを求める。multiline() が true の場合 width を固定すると最適な height が求まる。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// ボタンをクリックしたイベント。
	Listener<Button::Click&>& onClick();
	/// 描画するイベント。
	Listener<Button::Paint&>& onPaint();
	/// ボタンを押したイベントを発生させる。
	void performClick();

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

private:
	Listener<Button::Click&> _onClick;
	Listener<Button::Paint&> _onPaint;
};



	}
}