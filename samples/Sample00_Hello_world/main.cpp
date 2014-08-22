#include <Windows.h>
#include <balor/gui/all.hpp>

using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) 
{
	// 메인 윈도우를 만들고, 타이틀 이름을 "Hello world"로 설정
	Frame frame(L"Hello world");

	// Win32의 OnPaint 메시지에서 20,10 좌표에 "Hello world"를 출력
	frame.onPaint() = [&] (Frame::Paint& e) {
		e.graphics().drawText(L"Hello world", 20, 10);
	};

	// Win32 메시지 처리
	frame.runMessageLoop();

	return 0;
}
