#include "StdAfx.h"
#include "Sample.hpp"

//#include <Windows.h>

namespace balor {
	namespace Convert {

	template<> String to<String>(const Rectangle& value, int ) {
		return String() + L" (" + value.x + L", " + value.y + L", " + value.width + L", " + value.height + L")";
	}

	template<> String to<String>(const Size& value, int ) {
		return String() + L" (" + value.width + L", " + value.height + L")";
	}

	template<> String to<String>(const Point& value, int ) {
		return String() + L" (" + value.x + L", " + value.y + L")";
	}

	}
}


namespace sample {

using std::wstring;
using std::string;
using std::vector;
using std::move;
using boost::thread;

using namespace balor::graphics;
using namespace balor::gui;
using namespace balor::io;
using namespace balor::locale;
using namespace balor::system;
using namespace balor::test;
using namespace balor;

using namespace balor::Convert;


Sample::Sample() {
}


Sample::~Sample() {
}


void test() {
	//int* p = new int[2];
}


void Sample::main() {
	test();

	Frame frame(L"sample", 1000, 800, Frame::Style::sizable);
	frame.helpButton(true);

	PopupMenu framePopupMenu;
	Bitmap bitmap0 = Icon::asterisk().bitmap().clone();
	{
		typedef Menu::ItemInfo Item;
		Item itemsSub[] = {
			Item(L"sframe체크하기", [&] (Menu::Event& e) { e.sender().checked(!e.sender().checked()); })
			,Item(L"sframe편집", [&] (Menu::Event& ) { MsgBox::show(L"s편집"); })
			,Item()
			,Item(L"sframe더미", [&] (Menu::Event& ) { MsgBox::show(L"s더미"); })
		};
		Item items[] = {
			 Item(L"frame체크하기", [&] (Menu::Event& e) { e.sender().checked(!e.sender().checked()); })
			,Item(L"frame편집", [&] (Menu::Event& ) { MsgBox::show(L"편집"); })
			,Item(L"frame더미", itemsSub)
		};
		framePopupMenu.itemInfos(items);
		framePopupMenu[0].uncheckedMark(bitmap0);
		framePopupMenu[2].userData(String(L"응?"));
		frame.onPopupMenu() = [&] (Control::PopupMenu& e) {
			framePopupMenu.show(frame, e.position());
		};
	}
	frame.onShortcutKey() += [&] (Control::ShortcutKey& e) {
		switch (e.shortcut()) {
			case Key::Modifier::ctrl | Key::b : MsgBox::show(L"frameCTRL + B"); e.handled(true); e.isInputKey(false); break;
		}
	};
	Brush frameBrush(Color::green());
	//frame.brush(frameBrush);
	//frame.onPaint() = [&] (Frame::Paint& e) {
	//	Brush brush(Color::red());
	//	e.graphics().brush(brush);
	//	e.graphics().clear();
	//};
	frame.onMouseDown() = [&] (Control::MouseDown& ) {
		Debug::writeLine(L"frameOnMouseDown");
	};
	frame.onMouseWheel() = [&] (Control::MouseWheel& ) {
		Debug::writeLine(L"frameOnMouseWheel");
	};

	LineLayout layout(frame);

	frame.onResize() = [&] (Frame::Resize& ) {
		layout.perform();
	};

	Scaler s;
	frame.scale(s);
	layout.scale(s);
	layout.startPosition(0, 0);


	Panel page0(frame, 0, 0, 0, 0);
	Button button0(page0, 20, 10, 0, 0, L"button0", [&] (Button::Click& ) {
		MsgBox::show(L"button0");
	});
	Button button1(page0, 20, 50, 0, 0, L"button1", [&] (Button::Click& ) {
		MsgBox::show(L"button1");
	});
	Panel page1(frame, 0, 0, 0, 0);
	CheckBox check(page1, 20, 10, 0, 0, L"check");

	typedef Tab::ItemInfo Info;
	Info infos[] = {
		Info(L"탭0", page0),
		Info(L"탭1", page1),
		Info(L"탭2"),
	};
	Tab tab(frame, 20, 10, 400, 300, infos);
	//tab.itemInfos(infos);


	//auto& cparent = frame;

	{
	//layout.perform();

	//// 공통 테스트 루틴 
	//Bitmap drawTo(c.size());
	//Panel panel(cparent, 0, 0, 30 + c.bounds().width, 60 + c.bounds().height * 2, Control::Edge::client);
	//{// bounds
	//	auto old = c.bounds();
	//	c.bounds(Rect(100, 200, 300, 400));
	//	verify(c.bounds() == Rect(100, 200, 300, 400) || dynamic_cast<ComboBox*>(&c));
	//	c.bounds(old);
	//}
	//// drawTo
	//c.drawTo(drawTo);
	//panel.onPaint() = [&] (Panel::Paint& e) {
	//	e.graphics().brush(Color::control());
	//	e.graphics().clear();
	//	e.graphics().copy(10, c.bounds().bottom() + 20, Graphics(drawTo));
	//};
	//// enabled
	//Button enabled(cparent, 0, 0, 0, 0, L"enabled", [&] (Button::Event& ) {
	//	c.enabled(!c.enabled());
	//});
	//// clientSize
	//verify(c.clientSize() != Size(0, 0));
	//verify(c.clientSize().width  <= c.bounds().width );
	//verify(c.clientSize().height <= c.bounds().height);
	//verify(c.clientRectangle().position() == Point(0, 0));
	//verify(c.clientRectangle().size() == c.clientSize());
	//// clientSizeFromSize
	//verify(c.clientSizeFromSize(c.size()) == c.clientSize() || dynamic_cast<ListBox*>(&c));
	//// clientToScreen
	//verify(c.clientToScreen(Point(0, 0)) == cparent.clientToScreen(c.position() - (cparent.clientToScreen(c.position()) - c.clientToScreen(Point(0, 0)))));
	////verify(c.clientToScreen(c.clientRectangle()) == cparent.clientToScreen(c.bounds()));
	//// findFrame
	//verify(c.findFrame() == &frame);
	//// focus/focused/focusible
	//Button focus(cparent, 0, 0, 0, 0, L"focus", [&] (Button::Event& ) {
	//	verify(!c.focused());

	//	// focusible
	//	verify(c.focusable());
	//	c.visible(false);
	//	verify(!c.focusable());
	//	c.visible(true);
	//	verify(c.focusable());
	//	c.enabled(false);
	//	verify(!c.focusable());
	//	c.enabled(true);
	//	verify(c.focusable());

	//	c.focus();
	//	verify(c.focused());
	//});
	//// font
	//Font bigFont(L"메이리오", 40);
	//Button font(cparent, 0, 0, 0, 0, L"font", [&] (Button::Event& ) {
	//	c.font(c.font() == bigFont ? (Font::HFONT)Control::defaultFont() : (Font::HFONT)bigFont);
	//	c.resize();
	//	layout.perform();
	//});
	//// fromHandle
	//verify(&c == Control::fromHandle(c));
	//// invalidate
	//Button invalidate(cparent, 0, 0, 0, 0, L"invalidate", [&] (Button::Event& ) {
	//	c.invalidate();
	//	c.invalidate(true);
	//	c.invalidate(Rect(0, 0, 10, 10));
	//	c.invalidate(Rect(0, 0, 10, 10), true);
	//	c.invalidate(Region::fromEllipse(0, 0, 30, 30));
	//	c.invalidate(Region::fromEllipse(0, 0, 30, 30), true);
	//});
	//// invoke/invokeRequired
	//Button invoke(cparent, 0, 0, 0, 0, L"invoke", [&] (Button::Event& ) {
	//	verify(!c.invokeRequired());
	//	c.invoke([&] () {
	//		MsgBox::show(L"invoked!");
	//	});
	//	auto& cref = c;
	//	thread([&] () {
	//		verify(cref.invokeRequired());
	//		Debug::writeLine(L"invoked asynch!");
	//	});
	//});
	//// mouseHoverTime
	//Button mouseHoverTime(cparent, 0, 0, 0, 0, L"mouseHT", [&] (Button::Event& ) {
	//	c.mouseHoverTime(c.mouseHoverTime() == 100 ? 3000 : 100);
	//});
	//// mouseTracked
	//// name
	//c.name(L"c");
	//verify(c.name() == L"c");
	//// parent
	//verify(c.parent() == &cparent || (dynamic_cast<ComboBox*>(c.parent()) && c.findFrame() == &frame));
	//Button parent(cparent, 0, 0, 0, 0, L"parent", [&] (Button::Event& ) {
	//	c.parent(c.parent() == &cparent ? &panel : (Control*)&cparent);
	//});
	//{// position
	//	auto old = c.position();
	//	c.position(1, 2);
	//	verify(c.position() == Point(1, 2));
	//	c.position(old);
	//	verify(c.position() == c.bounds().position());
	//}
	//// region
	//Button region(cparent, 0, 0, 0, 0, L"region", [&] (Button::Event& ) {
	//	c.region(c.region() ? (Region::HRGN)nullptr : (Region::HRGN)Region::fromEllipse(Rect(Point(0, 0), c.size())));
	//});
	//// screenToClient
	//verify(c.screenToClient(cparent.clientToScreen(c.position())) == Point(0, 0) + (cparent.clientToScreen(c.position()) - c.clientToScreen(Point(0, 0))));
	////verify(c.screenToClient(cparent.clientToScreen(c.bounds())) == Rect(Point(0, 0), c.size()));
	//{// size
	//	auto old = c.size();
	//	c.size(300, 200);
	//	verify(c.size() == Size(300, 200) || dynamic_cast<ListBox*>(&c) || dynamic_cast<ComboBox*>(&c));
	//	c.size(old);
	//	verify(c.size() == c.bounds().size());
	//}
	//// sizeFromClientSize
	//verify(c.sizeFromClientSize(c.clientSize()) == c.size() || dynamic_cast<ListBox*>(&c));
	//// tabIndex
	//verify(c.tabIndex() == 0);
	//c.tabIndex(10);
	//verify(c.tabIndex() == 10);
	//c.tabIndex(0);
	//// tabStop
	//Button tabStop(cparent, 0, 0, 0, 0, L"tabStop", [&] (Button::Event& ) {
	//	c.tabStop(!c.tabStop());
	//});
	//// text
	//String textBackup;
	//Button text(cparent, 0, 0, 0, 0, L"text", [&] (Button::Event& ) {
	//	if (c.text() !=  L"text갱신") {
	//		textBackup = c.text();
	//	}
	//	c.text(c.text() != L"text갱신" ? L"text갱신" : textBackup);
	//});
	//// update
	//Button update(cparent, 0, 0, 0, 0, L"update", [&] (Button::Event& ) {
	//	c.update();
	//});
	//// visible
	//Button visible(cparent, 0, 0, 0, 0, L"visible", [&] (Button::Event& ) {
	//	c.visible(!c.visible());
	//});
	//// visualStyle
	//Button visualStyle(cparent, 0, 0, 0, 0, L"visualStyle", [&] (Button::Event& ) {
	//	c.visualStyle(!c.visualStyle());
	//});
	//// zOrder
	//// acceptButton
	//Button acceptButton(cparent, 0, 0, 0, 0, L"acceptButton", [&] (Button::Event& ) {
	//	MsgBox::show(L"accept!");
	//});
	//frame.acceptButton(&acceptButton);
	//// visualStyle
	//Button cancelButton(cparent, 0, 0, 0, 0, L"cancelButton", [&] (Button::Event& ) {
	//	MsgBox::show(L"cancelButton!");
	//});
	//frame.cancelButton(&cancelButton);

	//// events
	//c.onActivate() += [&] (Control::Activate& ) { Debug::writeLine(L"onActivate!"); };
	//c.onCursorChange() += [&] (Control::CursorChange& e) { e.cursor(Cursor::hand()); verify(e.cursor() == Cursor::hand()); };
	//c.onDeactivate() += [&] (Control::Event& ) { Debug::writeLine(L"onDeactivate!"); };
	//c.onDefocus() += [&] (Control::Defocus& ) { Debug::writeLine(L"onDefocus!"); };
	//c.onMouseDoubleClick() += [&] (Control::MouseDoubleClick& e) { Debug::writeLine(String(L"MouseDoubleClick!") + (int)e.button()); };
	//c.onFocus() += [&] (Control::Focus& ) { Debug::writeLine(L"onFocus!"); };
	//c.onDrag() += [&] (Control::Drag& ) { Debug::writeLine(L"onDrag!"); };
	//c.onHelpRequest() += [&] (Control::HelpRequest& e) {
	//	verify(c.clientToScreen(e.position()) == Mouse::position());
	//	verify(&e.target() == &c);
	//	e.cancel(MsgBox::show(L"helpRequest?", L"help", MsgBox::Buttons::okCancel) == MsgBox::Result::cancel);
	//};
	//cparent.onHelpRequest() += [&] (Control::HelpRequest& e) {
	//	verify(&e.target() == &c);
	//	Debug::writeLine(L"onHelpRequest at Frame!");
	//};
	//c.onKeyDown() += [&] (Control::KeyDown& e) {
	//	Debug::writeLine(String(L"onKeyDown![") + Key::toChar(e.key()) + L"]:" + e.previousDown());
	//};
	//c.onKeyPress() += [&] (Control::KeyPress& e) {
	//	Debug::writeLine(String(L"onKeyPress![") + e.charCode() + L"]:");
	//};
	//c.onKeyUp() += [&] (Control::KeyUp& e) {
	//	Debug::writeLine(String(L"onKeyUp![") + Key::toChar(e.key()) + L"]:" + e.previousDown());
	//};
	//c.onMouseDown() += [&] (Control::MouseDown& e) { Debug::writeLine(String(L"MouseDown!") + (int)e.button()); };
	//c.onMouseEnter() += [&] (Control::Event& ) { Debug::writeLine(L"onMouseEnter!"); };
	//c.onMouseHover() += [&] (Control::Event& ) { Debug::writeLine(L"onMouseHover!"); };
	//c.onMouseLeave() += [&] (Control::Event& ) { Debug::writeLine(L"onMouseLeave!"); };
	//c.onMouseUp() += [&] (Control::MouseUp& e) { Debug::writeLine(String(L"MouseUp!") + (int)e.button()); };
	//c.onMouseWheel() += [&] (Control::MouseWheel& e) { Debug::writeLine(String(L"MouseWheel!") + e.wheelDelta()); };
	//PopupMenu popupMenu;
	//{
	//	typedef Menu::ItemInfo Item;
	//	Item items[] = {
	//		 Item(L"체크하기", [&] (Menu::Event& e) { e.sender().checked(!e.sender().checked()); })
	//		,Item(L"편집", [&] (Menu::Event& ) { MsgBox::show(L"편집"); })
	//		,Item(L"더미", [&] (Menu::Event& ) { MsgBox::show(L"더미"); })
	//	};
	//	popupMenu = PopupMenu(items);
	//}
	//c.onPopupMenu() += [&] (Control::PopupMenu& e) { popupMenu.show(e.sender(), e.position()); };
	//c.onShortcutKey() += [&] (Control::ShortcutKey& e) {
	//	switch (e.shortcut()) {
	//		case Key::Modifier::ctrl | Key::a : MsgBox::show(L"CTRL + A"); e.handled(true); e.isInputKey(false); break;
	//	}
	//};

	frame.runMessageLoop();
	}
	frame = Frame();
}


}
