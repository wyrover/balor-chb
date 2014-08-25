#pragma once

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {


	/**
	* Button, CheckBox, RadioButton의 공통 처리를 모아 놓은 기본 클래스
	*/
	class ButtonBase : public Control {
	public:
		/// 문자열 배치
		struct TextAlign {
			enum _enum {
				topLeft = 0x00000500L, /// 좌상
				topCenter = 0x00000700L, /// 상 중앙
				topRight = 0x00000600L, /// 우상
				middleLeft = 0x00000D00L, /// 중앙 좌
				middleCenter = 0x00000F00L, /// 중앙
				middleRight = 0x00000E00L, /// 중앙 우
				bottomLeft = 0x00000900L, /// 좌하
				bottomCenter = 0x00000B00L, /// 하중앙
				bottomRight = 0x00000A00L, /// 우하
			};
			BALOR_NAMED_ENUM_MEMBERS(TextAlign);
		};


	protected:
		ButtonBase();
		ButtonBase(ButtonBase&& value);
		virtual ~ButtonBase() = 0;

	public:
		/// 문자열이 삐져 나오는 경우나 개행 코드에 의해서 개항할지 어떨지. 초기값은 false
		bool multiline() const;
		void multiline(bool value);
		/// 버튼을 누른 상태인지 어떤지
		bool pushed() const;
		/// 문자열 배치. 초기값은 Button은 ButtonBase::TextAlign::middleCenter, CheckBox와 RadioButton는 middleLeft。
		ButtonBase::TextAlign textAlign() const;
		void textAlign(ButtonBase::TextAlign value);
	};



	/**
	* 버튼 컨트롤
	*
	* 체크 박스, 라디오 버튼, 그룹 박스는 각각 CheckBox, RadioButton, GroupBox 클래스로 구현되어 있다.
	* BS_BITMAP나 BS_ICON 스타일은 XP에서 비주얼 스타일이 강제적으로 무효화 되므로 지원하지 않는다.
	* 커스텀 draw나 BCM_SETIMAGELIST는 강제적으로 페이드 애니메이션 되어버려서 지원하지 않는다. onPaint 이벤트에서 대응한다.
	*
	* <h3> 샘플 코드 </h3>
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
		/// Button 이벤트의 부모 클래스.
		typedef EventWithSubclassSender<Button, Control::Event> Event;

		typedef Event Click;
		typedef PaintEvent<Button, Event> Paint;


	public:
		/// null 핸들로 만든다.
		Button();
		Button(Button&& value, bool checkSlicing = true);
		/// 부모, 위치, 크기, 문자열로 만든다. 크기를 0으로 하면 getPreferredSize 함수로 구한다.
		Button(Control& parent, int x, int y, int width, int height, StringRange text, Listener<Button::Event&> onClick = Listener<Button::Event&>());
		virtual ~Button();
		Button& operator=(Button&& value);

	public:
		/// Enter key를 눌렀을 때에 포커스가 없었으면 눌런 것으로 되는 버튼의 외견을 어떻게 할지. 초기 값은 false
		bool defaultButton() const;
		void defaultButton(bool value);
		/// 현재 문자열이나 폰트 등에서 최적인 크기를 구한다. multiline()이 true 인 경우 width를 고정하면 최적의 height를 구한다.
		virtual Size getPreferredSize(int width = 0, int height = 0) const;
		/// 버튼 클릭 이벤트
		Listener<Button::Click&>& onClick();
		///랜더링 하는 이벤트
		Listener<Button::Paint&>& onPaint();
		/// 버튼을 클릭 이벤트를 발생 시킨다.
		void performClick();

	protected:
		/// 메시지를 처리한다. 이를테면 윈도우 프로시져 
		virtual void processMessage(Message& msg);

	private:
		Listener<Button::Click&> _onClick;
		Listener<Button::Paint&> _onPaint;
	};



	}
}