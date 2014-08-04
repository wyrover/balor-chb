#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {



/**
* スクロール機能をもったコントロールの基底クラス。
*
* Bitmap クラスを表示するウインドウで Bitmap をスクロールして表示できるようにするには次のように設定する。。
* <pre><code>
*	panel.scrollMinSize(bitmap.size());
* </code></pre>
* 子コントロールが全て表示できるようにスクロール範囲を設定するには次のように設定する。
* <pre><code>
*	frame.scrollMinSize(frame.clientSizeFromSize(frame.preferredSize()));
* </code></pre>
*/
class ScrollableControl : public Control {
public:
	/// ScrollableControl のイベントの親クラス。
	typedef EventWithSubclassSender<ScrollableControl, Control::Event> Event;


	/// スクロール位置が変化したイベント。
	struct Scroll : public ScrollableControl::Event {
		Scroll(ScrollableControl& sender, const Point& oldPosition, const Point& newPosition);

		/// 新しいスクロール位置。
		const Point& newPosition() const;
		/// 古いスクロール位置。
		const Point& oldPosition() const;

	private:
		Point _oldPosition;
		Point _newPosition;
	};


protected: // コントロールの生成と破棄
	ScrollableControl();
	ScrollableControl(ScrollableControl&& value);
	virtual ~ScrollableControl() = 0;

public:
	/// クライアント領域の大きさ。
	using Control::clientSize;
	virtual void clientSize(const Size& value);
	/// スクロール位置やスクロール範囲を考慮した仮想的なコントロールのクライアント座標と大きさ。
	virtual Rectangle displayRectangle() const;
	/// 水平スクロールバーが表示されているかどうか。
	bool hScrollBar() const;
	/// 水平スクロールバーを常に表示するかどうか。初期値は false。動的に変更すると挙動がおかしくなることがあるので最初に一度だけ設定すること。
	bool hScrollBarFixed() const;
	void hScrollBarFixed(bool value);
	/// スクロール位置が変化したイベント。
	Listener<ScrollableControl::Scroll&>& onScroll();
	/// Scaler を使ってコントロールをスケーリングする。
	virtual void scale(const Scaler& scaler);
	/// control が表示されるようにスクロールする。recursive が true ならスクロール可能なコントロールが入れ子になっている場合にすべての階層でスクロールする。
	void scrollControlIntoView(Control& control, bool recursive = false);
	/// コントロールの大きさがこれ以下ならスクロールバーを表示する大きさ。0 ならばスクロールバーは表示されない。初期値は Size(0, 0)。
	Size scrollMinSize() const;
	void scrollMinSize(const Size& value);
	void scrollMinSize(int width, int height);
	/// スクロール位置。スクロールしていれば負数になる。
	Point scrollPosition() const;
	void scrollPosition(const Point& value);
	void scrollPosition(int x, int y);
	/// 現在のスクロール範囲の大きさ。
	Size scrollSize() const;
	/// 垂直スクロールバーが表示されているかどうか。
	bool vScrollBar() const;
	/// 垂直スクロールバーを常に表示するかどうか。初期値は false。動的に変更すると挙動がおかしくなることがあるので最初に一度だけ設定すること。
	bool vScrollBarFixed() const;
	void vScrollBarFixed(bool value);

protected:
	/// スクロールバーの状態を更新する。
	virtual void updateScrollBars();
	virtual void processMessage(Message& msg);

protected:
	Size _scrollMinSize;
	Size _scrollSize;
	bool _hScrollBarFixed;
	bool _vScrollBarFixed;
	bool _updating;
	Listener<ScrollableControl::Scroll&> _onScroll;
};



	}
}