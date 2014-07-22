#pragma once


namespace balor {



/**
 * すべてのイベントクラスの親クラス。
 *
 * Event の参照を引数にとる関数オブジェクトは Event クラスの派生クラスの参照を引数にとるあらゆるイベントリスナーに登録することが出来る。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"Event Sample");

	Button button(frame, 20, 10, 0, 0, L"ボタン");
	CheckBox check(frame, 20, 50, 0, 0, L"チェック");

	// Event& を引数に取る関数オブジェクトはあらゆるイベントに代入できる。
	auto onAny = [&] (Event& ) {
		MsgBox::show(L"something event");
	};
	button.onClick() = onAny;
	check.onStateChange() = onAny;

	frame.runMessageLoop();
 * </code></pre>
 */
class Event {
};


/// イベント発生源を伴うイベントクラス。
template<typename Sender>
class EventWithSender : public Event {
public:
	/// イベント発生源を指定して作成。
	EventWithSender(Sender& sender) : _sender(sender) {}

private: // 複製の禁止。
	EventWithSender(const EventWithSender& );
	EventWithSender& operator=(const EventWithSender& );

public:
	/// イベント発生源。
	Sender& sender() { return _sender; }
	const Sender& sender() const { return _sender; }

private:
	Sender& _sender;
};


/// sender 関数を派生クラスで上書きするイベントクラス。
template<typename SubclassSender, typename BaseEvent>
class EventWithSubclassSender : public BaseEvent {
public:
	/// イベント発生源を指定して作成。
	EventWithSubclassSender(SubclassSender& sender) : BaseEvent(sender) {}

public:
	/// イベント発生源。
	SubclassSender& sender() { return static_cast<SubclassSender&>(BaseEvent::sender()); }
	const SubclassSender& sender() const { return static_cast<const SubclassSender&>(BaseEvent::sender()); }
};


}