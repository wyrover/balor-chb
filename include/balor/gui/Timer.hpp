#pragma once

#include <balor/Event.hpp>
#include <balor/Listener.hpp>
#include <balor/Reference.hpp>

struct HWND__;


namespace balor {
	namespace gui {

class Control;


/**
 * タイマーを表す。
 *
 * start() 関数でタイマーを開始すると指定した実行間隔で onRun イベントを発生させる。
 * onRun イベントが実行されるのは executor() が属するメッセージループと同じスレッドになる。
 * executor()  がメッセージループで処理されていないとタイマーは動かない。
 * Timer への操作はスレッドセーフではないので、複数のスレッドから Timer を操作する場合は自分でロックを行うこと。
 * タイマーの精度は 55 ミリ秒程度で、メッセージが混雑している時には処理されない場合もある。
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
class Timer : private NonCopyable {
public:
	/// タイマーのイベントクラス。
	typedef EventWithSender<Timer> Run;

public:
	/// 空タイマーを作成。
	Timer();
	Timer(Timer&& value);
	/// 実行する Control、実行間隔、onRun イベントから作成。
	Timer(Control& executor, int interval, Listener<Timer::Run&> onRun = Listener<Timer::Run&>());
	~Timer();
	Timer& operator=(Timer&& value);

public:
	/// タイマーを実行する Control。
	Control* executor() const;
	/// onRun の実行間隔（ミリ秒）。
	int interval() const;
	void interval(int value);
	/// interval ごとに発生するイベント。
	Listener<Timer::Run&>& onRun();
	/// タイマーを開始する。
	void start();
	/// 開始済みかどうか。初期値は false。
	bool started() const;
	/// タイマーを停止する。
	void stop();

private:
	typedef ::HWND__* HWND;

	Reference<Control> _executor;
	int _interval;
	Listener<Timer::Run&> _onRun;
	bool _started;
};



	}
}