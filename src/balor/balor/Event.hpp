#pragma once


namespace balor {



	/**
	* 전체 이벤트 클래스의 부모 클래스
	*
	* Event 참조를 인수로 취하는 함수 오브젝트는 Event 클래스 파생 클래스의 참조를 인수로 취하는 모든 이벤트 리스너에 등록해 두는 것이 가능하다.
	*
	* <h3>・샘플코드</h3>
	* <pre><code>
	Frame frame(L"Event Sample");

	Button button(frame, 20, 10, 0, 0, L"버튼");
	CheckBox check(frame, 20, 50, 0, 0, L"체크");

	// Event&를 인수로 취하는 함수 오브젝트는 모두 이벤트로 대입할 수 있다.
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


/// 이벤트 발신자를 포함하는 이벤트 클래스
template<typename Sender>
class EventWithSender : public Event {
public:
	/// 이벤트 발신자를 지정해서 작성.
	EventWithSender(Sender& sender) : _sender(sender) {}

private: // 복사 금지.
	EventWithSender(const EventWithSender& );
	EventWithSender& operator=(const EventWithSender& );

public:
	/// 이벤트 발신자.
	Sender& sender() { return _sender; }
	const Sender& sender() const { return _sender; }

private:
	Sender& _sender;
};


/// sender 함수를 파생 클래스로 덮어쓰는 이벤트 클래스.
template<typename SubclassSender, typename BaseEvent>
class EventWithSubclassSender : public BaseEvent {
public:
	/// 이벤트 발신자를 지정해서 작성.
	EventWithSubclassSender(SubclassSender& sender) : BaseEvent(sender) {}

public:
	/// 이벤트 발생자.
	SubclassSender& sender() { return static_cast<SubclassSender&>(BaseEvent::sender()); }
	const SubclassSender& sender() const { return static_cast<const SubclassSender&>(BaseEvent::sender()); }
};


}