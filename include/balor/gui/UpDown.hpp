#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {

class Edit;



/**
 * アップダウンコントロール。
 * 
 * 相棒コントロールの位置や大きさが変化した場合、自動的に追従はしないので再度 buddy 関数で設定してやればよい。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"UpDown Sample");

	int number = 0;
	Edit edit(frame, 20, 10, 0, 25, 10);
	edit.text(String() + number);
	UpDown upDown(frame, edit);
	upDown.onUp() = [&] (UpDown::Up& ) {
		++number;
		edit.text(String() + number);
	};
	upDown.onDown() = [&] (UpDown::Down& ) {
		--number;
		edit.text(String() + number);
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class UpDown : public Control {
public:
	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none        = 0     , 
			noArrowKeys = 0x0020, /// buddy() から矢印キーやマウスホイールの入力を受け取らないようにする。
			horizontal  = 0x0040, /// 左右ボタンのアップダウンコントロールにする。
			leftAlign   = 0x0008, /// buddy() の左に吸着する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// アップダウンコントロールのイベントの親クラス。
	typedef EventWithSubclassSender<UpDown, Control::Event> Event;

	typedef Event Down;
	typedef Event Up;


public:
	/// ヌルハンドルで作成。
	UpDown();
	UpDown(UpDown&& value, bool checkSlicing = true);
	/// 親コントロール、位置、大きさ、最小位置、最大位置から作成。その他の引数については同名の関数を参照。
	UpDown(Control& parent, int x, int y, int width, int height, UpDown::Options options = Options::none);
	/// 親コントロール、相棒コントロールから作成。相棒コントロールに吸着する。その他の引数については同名の関数を参照。
	UpDown(Control& parent, Control& buddy, UpDown::Options options = Options::none);
	virtual ~UpDown();
	UpDown& operator=(UpDown&& value);

public:
	/// 相棒コントロール。このコントロールに吸着し、矢印キーやマウスホイールの入力をもらって onUp, onDown イベントを発生させる。
	/// 相棒コントロールの位置や大きさが変わっても追従はしないが再度設定しなおすと吸着する。
	Control* buddy() const;
	void buddy(Control* value);
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// 下ボタンを押したイベントのリスナー。
	Listener<UpDown::Down&>& onDown();
	/// 上ボタンを押したイベントのリスナー。
	Listener<UpDown::Up&>& onUp();
	///	コントロール作成後に変更できない設定。
	UpDown::Options options() const;

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	Size _buddySize;
	Listener<UpDown::Down&> _onDown;
	Listener<UpDown::Up&> _onUp;
};



	}
}