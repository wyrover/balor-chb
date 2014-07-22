#pragma once

#include <balor/gui/ScrollableControl.hpp>


namespace balor {
	namespace gui {



/**
 * コントロールを乗せてグループ化する親コントロール。
 *
 * fromParentHandle() 関数を使って Control の派生クラスではない任意のウインドウハンドルの子になることができる。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"Panel Sample");

	Label label(frame, 20, 10, 0, 0, L"ボタンを Panel に乗せて常にウインドウの右下に表示する");
	Panel bottom(frame, 0, 0, 0, 50);
	Panel right(bottom, 0, 0, 60, 50);
	Button ok(right, 20, 10, 0, 0, L"OK");

	DockLayout layout(frame);
	layout.setStyle(bottom, DockLayout::Style::bottom);
	DockLayout subLayout(bottom);
	subLayout.setStyle(right, DockLayout::Style::right);
	layout.perform();
	subLayout.perform();
	frame.onResized() = [&] (Frame::Resized& ) {
		layout.perform();
		subLayout.perform();
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class Panel : public ScrollableControl {
public:
	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none        = 0      ,
			transparent = 0x0020L, /// 透明で親コントロールが透けて見えるようにする。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// Panel のイベントの親クラス。
	typedef EventWithSubclassSender<Panel, ScrollableControl::Event> Event;

	typedef PaintEvent<Panel, Event> Paint;


public:
	/// ヌルハンドルで作成。
	Panel();
	Panel(Panel&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。その他の引数については同名の関数を参照。
	Panel(Control& parent, int x, int y, int width, int height, Control::Edge edge = Control::Edge::none, Panel::Options options = Options::none);
	virtual ~Panel();
	Panel& operator=(Panel&& value);

public:
	/// コントロールの境界線の種類。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// 親ウインドウハンドル、位置、大きさからコントロールを作成。その他の引数については同名の関数を参照。
	/// Control クラスの派生クラスではない任意のウインドウハンドルの子になることができる。
	static Panel fromParentHandle(HWND parent, int x, int y, int width, int height, Control::Edge edge = Control::Edge::none, Panel::Options options = Options::none);
	/// クライアント領域を描画するイベント。
	Listener<Panel::Paint&>& onPaint();
	///	コントロール作成後に変更できない設定。
	Panel::Options options() const;

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	Listener<Panel::Paint&> _onPaint;
};



	}
}