#include <Windows.h>
#include <balor/gui/all.hpp>

using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) 
{
	Frame frame(L"Hello world");
	frame.onPaint() = [&] (Frame::Paint& e) {
		e.graphics().drawText(L"Hello world", 20, 10);
	};
	frame.runMessageLoop();
	return 0;
}
