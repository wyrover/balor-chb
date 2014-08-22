#include <Windows.h>
#include <balor/gui/all.hpp>

using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) 
{
	// ���� �����츦 �����, Ÿ��Ʋ �̸��� "Hello world"�� ����
	Frame frame(L"Hello world");

	// Win32�� OnPaint �޽������� 20,10 ��ǥ�� "Hello world"�� ���
	frame.onPaint() = [&] (Frame::Paint& e) {
		e.graphics().drawText(L"Hello world", 20, 10);
	};

	// Win32 �޽��� ó��
	frame.runMessageLoop();

	return 0;
}
