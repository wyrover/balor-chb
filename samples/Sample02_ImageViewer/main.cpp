#include <Windows.h>
#include <balor/graphics/all.hpp>
#include <balor/gui/all.hpp>

using namespace balor::graphics;
using namespace balor::gui;


int APIENTRY WinMain(HINSTANCE , HINSTANCE , LPSTR , int ) 
{
	Frame frame(L"ImageViewer");
	Bitmap bitmap;

	typedef Menu::ItemInfo Item;
	Item fileMenuItems[] = {
		Item(L"열기(&O)\tCtrl+O", Key::Modifier::ctrl | Key::o),
		Item(),
		Item(L"종료(&X)\tAlt+F4", Key::Modifier::alt | Key::f4)
	};
	Item menuBarItems[] = {
		Item(L"파일(&F)", fileMenuItems),
		Item(L"도움말(&H)", [&] (Menu::Click& ) { MsgBox::show(L"Image Viewer Ver 1.0", L"버전 정보"); })
	};
	MenuBar menuBar(menuBarItems);
	frame.menuBar(&menuBar);

	frame.onShortcutKey() = [&] (Frame::ShortcutKey& e) {
		switch (e.shortcut()) {
			case Key::Modifier::ctrl | Key::o : {
				OpenFileDialog dialog;
				dialog.filter(L"그림 파일\n*.bmp;*.gif;*.png;*.jpg;*.jpeg;*.tiff\n\n");
				if (dialog.show(frame)) {
					bitmap = Bitmap(dialog.filePath());
					if (bitmap != nullptr) {
						frame.scrollMinSize(bitmap.size()); // 윈도우 사이즈가 그림 사이즈보다 작으면 스크롤 할 수 있도록 한다
						frame.invalidate();
					}
				}
				e.handled(true);
			} break;
			case Key::Modifier::alt | Key::f4 : {
				frame.close();
				e.handled(true);
			} break;
		}
	};
	frame.onPaint() = [&] (Frame::Paint& e) {
		e.graphics().clear();
		if (bitmap != nullptr) {
			Graphics bitmapGraphics(bitmap);
			e.graphics().copy(frame.scrollPosition(), bitmapGraphics); // 스크롤을 빼고 이미지를 복사한다.
		}
	};

	frame.runMessageLoop();
	return 0;
}
