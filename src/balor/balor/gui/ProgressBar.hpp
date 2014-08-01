#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {



/**
 * プログレスバー。
 * 
 * ビジュアルスタイルが有効である場合、ウインドウスタイルの PBS_SMOOTH フラグの指定の有無にかかわらず XP では進捗バーに目盛りが表示され、Vista では目盛りが表示されない。
 * minimum を maximum より大きくしても maximum = minimum になるだけで増加量の符号は変わらない。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"ProgressBar Sample");

	ProgressBar bar(frame, 20, 10, 0, 0);
	Timer timer(frame, 500);
	Button button(frame, 20, 50, 0, 0, L"処理を開始する", [&] (Button::Click& e) {
		if (!timer.started()) {
			bar.value(0);
			timer.start();
			e.sender().text(L"処理を中止する");
			frame.onCursorChange() = [&] (Frame::CursorChange& e) {
				e.cursor(Cursor::wait());
			};
		} else {
			timer.stop();
			e.sender().text(L"処理を開始する");
			frame.onCursorChange().pop();
		}
		for (auto i = frame.descendantsBegin(); i; ++i) {
			i->onCursorChange() = frame.onCursorChange();
		}
	});
	timer.onRun() = [&] (Timer::Run& e) {
		bar.add(10);
		if (bar.maximum() <= bar.value()) {
			button.performClick();
			MsgBox::show(L"完了");
		}
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class ProgressBar : public Control {
public:
	/// ヌルハンドルで作成。
	ProgressBar();
	ProgressBar(ProgressBar&& value, bool checkSlicing = true);
	/// 親コントロール、位置、大きさ、開始値、完了値で作成。大きさを０にすると getPreferredSize 関数で求める。
	ProgressBar(Control& parent, int x, int y, int width, int height, int minimum = 0, int maximum = 100);
	virtual ~ProgressBar();
	ProgressBar& operator=(ProgressBar&& value);

public:
	/// 現在の進捗値に加算する。
	void add(int delta);
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// 現在の状態から最適な大きさを求める。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 進捗がいつ終わるかわからない表示モード（マーキースタイル）。アニメーションスピードはミリ秒単位。初期値は false。
	/// この関数の使用には EnableVisualStyle.hpp のインクルードが必要。
	bool indeterminate() const;
	void indeterminate(bool value, int animationSpeed = 100);
	/// 進捗の完了値。
	int maximum() const;
	void maximum(int value);
	/// 進捗の開始値。
	int minimum() const;
	void minimum(int value);
	/// 現在の進捗値。
	int value() const;
	void value(int value);
	/// 縦表示かどうか。
	bool vertical() const;
	void vertical(bool value);
};



	}
}