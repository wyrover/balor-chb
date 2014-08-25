#include <Windows.h>
#include <balor/gui/all.hpp>

using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) 
{
	Frame frame(L"ButtonAndEvent");

	// 버튼 생성
	Button button0(frame, 10, 10, 100, 50, L"button0");
	
	// 버튼 클릭 이벤트 설정
	button0.onClick() = [&] (Button::Click& ) {
		MsgBox::show(L"button0 clicked");
	};
	
	// 버튼 생성하면서 클릭이벤트도 설정
	Button button1(frame, 10, 80, 100, 50, L"button1", [&] (Button::Click& ) {
		MsgBox::show(L"button1 clicked");
	});

	
	frame.runMessageLoop();
	return 0;
}
