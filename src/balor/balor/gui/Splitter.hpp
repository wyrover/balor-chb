#pragma once

#include <balor/gui/Control.hpp>
#include <balor/gui/Padding.hpp>


namespace balor {
	namespace gui {



/**
* コントロールを分割してドラッグで接するコントロールの大きさを変えることができるコントロール。
*
* 縦長で作ると縦方向に分割し、横長で作ると横方向に分割する。
* 縦長ならば左右、横長ならば上下に接するコントロールの大きさをドラッグで変える事ができる。
* margin() でそれぞれの方向に接するコントロールの最小の大きさを指定できる。
*
* <h3>・サンプルコード</h3>
* <pre><code>
	Frame frame(L"Splitter sample", 1000, 800);

	Button left(frame, 0, 0, 100, 800, L"left");
	Splitter vertical(frame, 100, 0, 10, 800);
	Button bottom(frame, 110, 410, 890, 390, L"bottom");
	Splitter horizontal(frame, 110, 400, 890, 10);
	Button center(frame, 110, 0, 890, 400, L"center");

	vertical.margin(Padding(100));
	horizontal.margin(Padding(40));

	DockLayout dockLayout(frame);
	dockLayout.setStyle(left, DockLayout::Style::left);
	dockLayout.setStyle(vertical, DockLayout::Style::left);
	dockLayout.setStyle(bottom, DockLayout::Style::bottom);
	dockLayout.setStyle(horizontal, DockLayout::Style::bottom);
	dockLayout.setStyle(center, DockLayout::Style::fill);
	frame.onResized() = [&] (Frame::Resized& ) {
		dockLayout.perform();
	};

	frame.runMessageLoop();
* </code></pre>
*/
class Splitter : public Control {
public:
	/// Splitter のイベントの親クラス。
	typedef EventWithSubclassSender<Splitter, Control::Event> Event;

	typedef PaintEvent<Splitter, Event> Paint;


public:
	/// ヌルハンドルで作成。
	Splitter();
	Splitter(Splitter&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。その他の引数については同名の関数を参照。
	Splitter(Control& parent, int x, int y, int width, int height, Control::Edge edge = Control::Edge::none);
	virtual ~Splitter();
	Splitter& operator=(Splitter&& value);

public:
	/// 親のクライアント座標系での位置と大きさ。
	using Control::bounds;
	virtual void bounds(const Rectangle& value);
	/// コントロールの境界線の種類。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// それぞれの方向に接しているコントロールの大きさの最小値。初期値は Padding(0)。
	Padding margin() const;
	void margin(const Padding& value);
	/// クライアント領域を描画するイベント。
	Listener<Splitter::Paint&>& onPaint();
	/// 親コントロールのクライアント座標系におけるこのコントロールの位置。
	using Control::position;
	virtual void position(const Point& value);
	/// 縦方向に分割するかどうか。Splitter の size() が縦長ならば true。
	bool vertical() const;
	/// 表示優先度。小さいほど手前に表示される。0 を指定すると最前面に、-1 を指定すると最後面に移動する。
	using Control::zOrder;
	virtual void zOrder(int value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	bool _movable;
	bool _zOrderable;
	Padding _margin;
	Listener<Splitter::Paint&> _onPaint;
};



	}
}