#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {



/**
* スクロールバーコントロール。
*
* Scroll は pageSize, minimum, maximum の変更で value が変化した場合には発生しない。
* minimum を maximum より大きくしても maximum = minimum になるだけで増加量の符号は変わらない。value の最大値は maximum() - pageSize() + 1 になる。
* onMouseUp イベントは発生しない模様。それに関連してか onDrag イベントの発生タイミングもおかしくなっている。
*
* <h3>・サンプルコード</h3>
* <pre><code>
	Frame frame(L"ScrollBar Sample");

	ScrollBar scroll(frame, 20, 10, 0, 0, 0, 120, 21);
	Label label(frame, scroll.bounds().right() + 10, 10, 40, 0, String() + scroll.value());
	scroll.onScroll() = [&] (ScrollBar::Scroll& e) {
		label.text(String() + e.newValue());
	};

	frame.runMessageLoop();
* </code></pre>
*/
class ScrollBar : public Control {
public:
	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none = 0,
			vertical = 0x0001L, /// 垂直スクロールバーにする。初期は水平スクロールバー。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


public:
	/// スクロールバーのイベントの親クラス。
	typedef EventWithSubclassSender<ScrollBar, Control::Event> Event;


	/// スクロール位置が変化したイベント。
	struct Scroll : public Event {
		Scroll(ScrollBar& sender, int oldValue, int newValue, bool tracking);
		/// 新しいスクロール位置。
		int newValue() const;
		/// 古いスクロール位置。
		int oldValue() const;
		/// スクロールボックス（つまみ）をトラッキング中かどうか。
		bool tracking() const;

	private:
		int _newValue;
		int _oldValue;
		bool _tracking;
	};


public:
	/// ヌルハンドルで作成。
	ScrollBar();
	ScrollBar(ScrollBar&& value, bool checkSlicing = true);
	/// 親コントロール、位置、大きさ、最小位置、最大位置、スクロールボックス（つまみ）の大きさから作成。大きさを０にすると getPreferredSize 関数で求める。
	ScrollBar(Control& parent, int x, int y, int width, int height, int minimum = 0, int maximum = 120, int pageSize = 21, ScrollBar::Options options = Options::none);
	virtual ~ScrollBar();
	ScrollBar& operator=(ScrollBar&& value);

public:
	/// 水平スクロールバーの既定の高さ。
	static int defaultHorizontalHeight();
	/// 垂直スクロールバーの既定の幅。
	static int defaultVerticalWidth();
	/// 現在の状態から最適な大きさを求める。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 水平スクロールバーの矢印ボタンの幅。
	static int horizontalArrowWidth();
	/// 矢印キーまたはスクロールボタンを押したときまたはマウスホイールのスクロール量。初期値は１。
	int lineSize() const;
	void lineSize(int value);
	/// スクロールの最大位置。
	int maximum() const;
	void maximum(int value);
	/// スクロールの最小位置。
	int minimum() const;
	void minimum(int value);
	/// value が変化したイベントのリスナー。
	Listener<Scroll&>& onScroll();
	///	コントロール作成後に変更できない設定。
	ScrollBar::Options options() const;
	/// スクロールボックス（つまみ）の大きさ。
	int pageSize() const;
	void pageSize(int value);
	/// 現在のスクロール位置。minimum() から maximum() - pageSize() + 1 の間で変化する。
	int value() const;
	void value(int value);
	/// 垂直スクロールバーの矢印ボタンの高さ。
	static int verticalArrowHeight();

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	int _lineSize;
	Listener<Scroll&> _onScroll;
};



	}
}