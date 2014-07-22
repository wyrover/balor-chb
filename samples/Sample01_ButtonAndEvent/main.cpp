#include <Windows.h>
#include <balor/gui/all.hpp>

using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) {
	Frame frame(L"ButtonAndEvent");

	Button button0(frame, 10, 10, 100, 50, L"button0");
	button0.onClick() = [&] (Button::Click& ) {
		MsgBox::show(L"button0 clicked");
	};
	Button button1(frame, 10, 80, 100, 50, L"button1", [&] (Button::Click& ) {
		MsgBox::show(L"button1 clicked");
	});

	frame.runMessageLoop();
	return 0;
}
