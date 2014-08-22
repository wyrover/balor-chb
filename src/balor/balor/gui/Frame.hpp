#pragma once

#include <balor/gui/ScrollableControl.hpp>

struct tagMINMAXINFO;

namespace balor {
	namespace graphics {
		class Icon;
	}
}


namespace balor {
	namespace gui {

class Button;
class MenuBar;



/**
* Top 레벨로 다른 컨트롤의 프레임이 되는 윈도우.
*
* TODO: parent에 다른 컨트롤을 지정하여 자식 프레임이 되면 WS_CAPTION 스타일을 가지는 한 프레임 위의 에디터 컨트롤이 마우스에 반응하지 않는다. 원인불명 .NET에서도 같은 현상이 일어나므로 WIN32 사양인 듯한다.
*
* <h3> 샘플 코드 </h3>
* <pre><code>
Frame frame(L"Frame Sample");

Edit edit(frame, 20, 10, 0, 0, 20);
Button accept(frame, 20, 40, 0, 0, L"확인", [&] (Button::Click& ) 
{
	MsgBox::show(L"확인 하였습니다.");
});

Button cancel(frame, 20, 70, 0, 0, L"취소", [&] (Button::Click& ) 
{
	MsgBox::show(L"취소 하였습니다.");
});

frame.acceptButton(&accept); // Enter key에 반응
frame.cancelButton(&cancel); // ESC key에 반응

frame.onClosing() = [&] (Frame::Closing& e) 
{
	if (MsgBox::show(frame, L"종료합니까？", L"종료", MsgBox::Buttons::okCancel) != MsgBox::Result::ok) 
	{
		e.cancel(true);
	}
};

frame.icon(Icon::question());

frame.runMessageLoop();
* </code></pre>
*/
class Frame : public ScrollableControl {
public:
	typedef ::balor::graphics::Icon Icon;


	/// 윈도우를 닫는 이유
	struct CloseReason {
		enum _enum {
			none,
			taskManagerClosing, /// 태스크 매니져에서 닫았다
			userClosing, /// close() 메소드나 닫기 버튼으로 닫았다
			windowShutDown, /// 윈도우가 셧다운
		};
		BALOR_NAMED_ENUM_MEMBERS(CloseReason);
	};


	/// 윈도우 스타일
	struct Style {
		enum _enum {
			none, /// 타이틀 바도 틀도 없는 윈도우
			singleLine, /// 일중선을 가진 윈도우
			threeDimensional, /// 입체적인 틀을 가진 윈도우
			dialog, /// 다이얼로그 풍
			sizable, /// 리사이즈 할 수 있는 윈도우
			toolWindow, /// 툴 윈도우. 아이콘, 최대화 버튼, 최소화 버튼, 헬프 버튼이 표시되지 않는다.
			sizableToolWindow, /// 리사이즈 할 수 있는 툴 윈도우. 아이콘, 최대화 버튼, 최소화 버튼, 헬프 버튼이 표시되지 않는다.
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};


	/// Frame의 이벤트의 부모 클래스
	typedef EventWithSubclassSender<Frame, ScrollableControl::Event> Event;

	typedef Event DisplayChange;
	typedef Event MenuLoopBegin;
	typedef Event MenuLoopEnd;
	typedef Event ResizeMoveBegin;
	typedef Event ResizeMoveEnd;
	typedef PaintEvent<Frame, Event> Paint;


	/// 윈도우을 닫혔을 때의 이벤트
	struct Close : public Frame::Event {
		Close(Frame& sender, CloseReason reason);

		/// 閉じた理由。
		CloseReason closeReason() const;

	private:
		CloseReason _closeReason;
	};


	/// 윈도우를 닫는 중의 이벤트
	struct Closing : public Close {
		Closing(Frame& sender, CloseReason reason);

		/// 닫는 것을 취소할지 어떨지. 초기값은 false
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	///	헬프 버튼을 눌렀을 때의 이벤트
	struct HelpButtonClick : public Frame::Event {
		HelpButtonClick(Frame& sender);

		/// 기본 처리를 취소할지 어떨지. 초기값은 false
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	///	이동 이벤트
	struct Move : public Frame::Event {
		Move(Frame& sender, const Point& position);

		/// 이동한 스크린 위치
		const Point& position() const;

	protected:
		Point _position;
	};


	///	이동 중인 이벤트
	struct Moving : public Move {
		Moving(Frame& sender, const Point& position);

		/// 이동하는 스크린 위치
		using Move::position;
		void position(const Point& value);
		void position(int x, int y);
	};


	///	크기를 변경하는 이벤트
	struct Resizing : public Frame::Event {
		Resizing(Frame& sender, ::tagMINMAXINFO* info);

		/// 최대화 했을 때의 스크린 위치. ※이벤트명 미스매치!
		Point maximizedPosition() const;
		void maximizedPosition(const Point& value);
		void maximizedPosition(int x, int y);
		/// 최대화 했을 때의 크기
		Size maximizedSize() const;
		void maximizedSize(const Size& value);
		void maximizedSize(int width, int height);
		/// 윈도우를 드래그해서 크기를 변경 했을 때의 최대 크기
		Size maxTrackSize() const;
		void maxTrackSize(const Size& value);
		void maxTrackSize(int width, int height);
		/// 윈도우를 드래그 해서 크기를 변경했을 때의 최소 크기
		Size minTrackSize() const;
		void minTrackSize(const Size& value);
		void minTrackSize(int width, int height);

	private:
		::tagMINMAXINFO* _info;
	};


	///	크기를 변했을 때의 이벤트
	struct Resize : public Frame::Event {
		Resize(Frame& sender, int flag);

		/// 최대화 되었는지 어떤지
		bool maximized() const;
		/// 최소화 되었는지 어떤지
		bool minimized() const;

	private:
		int _flag;
	};


public:
	/// null 핸들로 만든다
	Frame();
	/// 타이틀 문자열, 클라이언트 사이즈, 스타일로 만든다. 클라이언트 사이즈를 0 으로 하면 기본 크기로 만든다.
	Frame(StringRange text, int clientWidth = 0, int clientHeight = 0, Frame::Style style = Style::sizable);
	Frame(Frame&& value, bool checkSlicing = true);
	virtual ~Frame();
	Frame& operator=(Frame&& value);

public:
	/// 버튼 이외에 포커스가 있을 때에 Enter 키를 누르면 눌렀던 일이 되는 버튼
	Button* acceptButton() const;
	void acceptButton(Button* value);
	/// 액티브화 한다
	void activate();
	/// 윈도우 상에서 포커스를 얻고 있는 컨트롤. 윈도우가 액티브 하지 않게 되었을 때도 보존되어 윈도우가 다시 액티브 되었을 때 포커스가 복원된다.
	virtual Control* activeControl() const;
	virtual void activeControl(Control* value);
	/// 현재 액티브한 윈도우
	static Frame* activeFrame();
	/// ESC key를 눌렀을 때 눌런 일을 하는 버튼
	Button* cancelButton() const;
	void cancelButton(Button* value);
	/// 타이틀 바의 문자열 폰트
	static Font captionFont();
	/// 오너 윈도우 중심으로 이동한다.
	void centerToOwner();
	/// 스크린 중신으로 이동한다
	void centerToScreen();
	/// 윈도우를 다는다. 윈도우은 비표시 되지만 파괴 되지 않는다
	void close();
	/// 데스크 탑 상의 좌표
	Point desktopPosition() const;
	void desktopPosition(const Point& value);
	void desktopPosition(int x, int y);
	/// 메시지 루프에서 나온다
	static void exitMessageLoop();
	/// 포커스를 얻었는지 어떤지
	virtual bool focusable() const;
	/// ？ 버튼을 유효화게 할지 어떨지. 유효화 한 경우 최대화 버튼과 최소화 버튼은 제거된다. 초기 값은 false
	bool helpButton() const;
	void helpButton(bool value);
	/// 타이틀 바에 표시하는 아이콘. 큰 아이콘을 지정한다. 초기 값은 nullptr.
	Icon icon() const;
	void icon(HICON value);
	/*bool keyPreview() const;  onShortcutKey 이벤트라면 필요 없다？ *//**/
	/*void keyPreview(bool value); *//**/
	/// 최대화 버튼을 유효화할지 어떨지. 초기 값은 true.
	bool maximizeButton() const;
	void maximizeButton(bool value);
	/// 최대화 하고 있는지 어떤지. visible이 true인 경우만 변경할 수 있다. 초기 값은 false
	bool maximized() const;
	void maximized(bool value);
	/// 메뉴 바를 설정한다.
	void menuBar(MenuBar* value);
	/// 메뉴 바가 점유하고 있는 영역. 없는 경우는 Rectangle(0, 0, 0, 0)를 반환한다
	Rectangle menuBarBounds() const;
	/// 메뉴 바가 포커스를 얻었는지 어떤지
	bool menuBarFocused() const;
	/// 최소화 버튼을 유효화 할지 어떨지. 초기 값은 true
	bool minimizeButton() const;
	void minimizeButton(bool value);
	/// 최소화 하고 있는지 어떤지. 초기 값은 false
	bool minimized() const;
	void minimized(bool value);
	/// 윈도우를 닫았을 때의 이벤트
	Listener<Frame::Close&>& onClose();
	/// 윈도우를 닫는 이벤트
	Listener<Frame::Closing&>& onClosing();
	/// 모니터의 해상도 또는 색수가 변경 되었을 때의 이벤트
	Listener<Frame::DisplayChange&>& onDisplayChange();
	/// 헬프 버튼을 눌렀을 때의 이벤트
	Listener<Frame::HelpButtonClick&>& onHelpButtonClick();
	/// 메뉴의 표시를 시작했을 때의 이벤트
	Listener<Frame::MenuLoopBegin&>& onMenuLoopBegin();
	/// 메뉴 표시를 종료 했을 때의 이벤트
	Listener<Frame::MenuLoopEnd&>& onMenuLoopEnd();
	/// 이동한 이벤트
	Listener<Frame::Move&>& onMove();
	/// 이동하고 있는 이벤트
	Listener<Frame::Moving&>& onMoving();
	/// 클라이언트 영역을 렌더링하는 이벤트
	Listener<Frame::Paint&>& onPaint();
	/// 크기가 변경 되었을 때의 이벤트
	Listener<Frame::Resize&>& onResize();
	/// 크기가 변경 되었을 때의 이벤트
	Listener<Frame::Resizing&>& onResizing();
	/// 사이즈 변경이나 이동을 시작했을 때의 이벤트
	Listener<Frame::ResizeMoveBegin&>& onResizeMoveBegin();
	/// 사이즈 변경이나 이동을 종료 했을 때의 이벤트
	Listener<Frame::ResizeMoveEnd&>& onResizeMoveEnd();
	/// 오너. 오너를 가진 윈도우는 보통 오너 보다 앞에 표시되어 오너가 최소화나 닫히는 것을 추종한다
	Frame* owner() const;
	void owner(Frame* value);
	/// 오너 윈도우 핸들. 오너를 가진 윈도우는 보통 오너 보다 앞에 표시되어 오너가 최소화나 닫히는 것을 추종한다
	HWND ownerHandle() const;
	void ownerHandle(HWND value);
	/// 부모 컨트롤
	using ScrollableControl::parent;
	virtual void parent(Control* value);
	/// setLayer 함수로 설정한 레이어를 무효화 한다
	void resetLayer();
	/// 최대화도 최소화도 하고 있지 않은 상태의 bounds
	Rectangle restoreBounds() const;
	/// 메시지 루프를 실행한다. exitMessageLoop 함수를 호출하거나 이 함수를 호출한 윈도우를 닫으면 루프를 빠져 나온다
	void runMessageLoop();
	/// 같은 스레드의 다른 윈도우를 사용 불가하고 메시지 루프를 실행한다. 이를테면 다이얼로그 표시. 오너를 설정할 필요가 있다.
	void runModalMessageLoop();
	/// 모인 메시지를 처리하여 메시지 루프를 종료해야 될지 어떨지를 반환한다
	bool runPostedMessages();
	/// 윈도우를 반투명화 한다. alpha는 불투명도(0～1)에서 1이 불투명. resetLayer 함수로 무효화 할 수 있다
	void setLayer(float alpha);
	/// 윈도우 상에서 colorKey와 같은 색 부분을 투명하게 한다. resetLayer 함수로 무효화 할 수 있다.
	void setLayer(const Color& colorKey);
	/// graphicsWithAlphaChannel로 지정한 알파채널을 가진 32비트 화상으로 윈도우와 배경을 반투명 합성한다.화상은 이미 Bitmap::premultiplyAlpha 함수를 실행해둘 필요가 있다
	/// alpha 인수에 더해 전제에 반투명을 걸칠 수도 있다. style()을 강제적으로 Frame::Style::none으로 설정한다. resetLayer 함수로 무효화 할 수 있다. 다른 setLayer 함수가 이미 적용되고 있는 경우 이 함수는 실패한다
	/// WM_PAINT가 처리되지 않고 화사만 표시 되므로 이 Frame 위에 컨트롤을 표시하고 싶은 경우는 이 Frame을 오너로 가진 colorKey로 투과한 Frame을 위에 띄워서 위치를 따라가도록 하는 것이 좋다
	void setLayer(HDC graphicsWithAlphaChannel, float alpha = 1.0f);
	/// 클라이언트 영역의 크기에서 컨트롤의 크기를 구한다
	virtual Size sizeFromClientSize(const Size& clientSize) const;
	/// 태스크 바에 표시될지 어떨지. 표시를 지우려면 style()이 Frame::Style::toolWindow 나 Frame::Style::sizableToolWindow 때나 오너를 가진 윈도우뿐
	/// 위의 style() 이외에서 표시를 지울 땨는 위의 style()을 가진 비표시 오너를 가지면 좋다. 초기 값은 true
	bool showInTaskbar() const;
	void showInTaskbar(bool value);
	/// 윈도우 스타일. 초기 값은 Frame::Style::sizable
	Frame::Style style() const;
	void style(Frame::Style value);
	/// 닫기, 최대화, 최소화 번튼과 시스템 메뉴를 유효하게 할지 어떨지. 초기 값은 true
	bool systemMenu() const;
	void systemMenu(bool value);
	/// 타이틀 문자열
	using ScrollableControl::text;
	virtual void text(StringRange value);
	/// 보통 최전면에 표시될지 어떨지. 초기 값은 false
	bool topMost() const;
	void topMost(bool value);
	/// 메뉴 바의 표시를 갱신한다. 팝업이 아닌 메뉴 항목을 변경했다면 갱신할 필요가 있다.
	void updateMenubar();
	/// 표시될지 어떨지. 초기 값은 false
	using ScrollableControl::visible;
	virtual void visible(bool value);


public: // 당초는 protected로 하고 싶었지만 Control과 그 파생 클래스에서 호출할 필요가 있어서 공개
	/// 자식 계 컨트롤이 자식 계가 아니게 될 때의 처리
	virtual void processDescendantErased(Control& descendant);
	///	자식 계 컨트롤이 포커스를 얻었을 때의 처리
	virtual void processDescendantFocused(Control& descendant);

protected:
	/// 다이얼로그 키를 처리해서 처리 할 수 있는지 어떤지를 반환한다
	virtual bool processDialogKey(int shortcut);
	/// 메시지를 처리한다. 이를테면 윈도우 프로시져
	virtual void processMessage(Message& msg);
	/// 윈도우 핸들 스타일을 갱신한다
	void updateHandleStyle(int style, int exStyle, HICON icon);


protected:
	bool _canPostQuitMessage;
	CloseReason _closeReason;
	Reference<Button> _acceptButton;
	Reference<Control> _activeControl;
	Reference<Button> _cancelButton;
	Style _style;
	Listener<Frame::Close&> _onClose;
	Listener<Frame::Closing&> _onClosing;
	Listener<Frame::DisplayChange&> _onDisplayChange;
	Listener<Frame::HelpButtonClick&> _onHelpButtonClick;
	Listener<Frame::MenuLoopBegin&> _onMenuLoopBegin;
	Listener<Frame::MenuLoopEnd&> _onMenuLoopEnd;
	Listener<Frame::Move&> _onMove;
	Listener<Frame::Moving&> _onMoving;
	Listener<Frame::Paint&> _onPaint;
	Listener<Frame::Resize&> _onResize;
	Listener<Frame::Resizing&> _onResizing;
	Listener<Frame::ResizeMoveBegin&> _onResizeMoveBegin;
	Listener<Frame::ResizeMoveEnd&> _onResizeMoveEnd;
};



	}
}