#pragma once

#include <balor/gui/Menu.hpp>

namespace balor {
class Point;
}


namespace balor {
	namespace gui {


/**
* ポップアップメニュー。
*
* TODO: ポップアップの表示がネストされているのを自動検出できないものか。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"PopupMenu Sample");

Label label(frame, 20, 10, 0, 0, L"右クリックでポップアップメニュー表示");

typedef Menu::ItemInfo Item;
Item items[] = {
Item(L"チェック(&C)", [&] (Menu::Click& e) {
auto& menuItem = e.sender();
menuItem.checked(!menuItem.checked());
}),
Item(L"警告する(&W)", [&] (Menu::Click& ) {
MsgBox::show(frame, L"警告");
}),
Item(),
Item(L"終了(&X)", [&] (Menu::Click& ) {
frame.close();
})
};
PopupMenu menu(items);
frame.onPopupMenu() = [&] (Frame::PopupMenu& e) {
menu.show(frame, e.position());
};

frame.runMessageLoop();
* </code></pre>
*/
class PopupMenu : public Menu {
public:
	/// 表示を制御するフラグ。組み合わせで指定する。
	struct Flags {
		enum _enum {
			recursive = 0x0001L, /// onRightClick() イベントで再帰的にポップアップメニューを表示する時に指定しなければならない。
			bothButton = 0x0002L, /// onRightClick() イベントの代わりに onClick() イベントを発生させる。
			hCentor = 0x0004L, /// 水平方向の中央に表示。
			right = 0x0008L, /// 右に表示。
			bottom = 0x0020L, /// 上に表示。
			vCentor = 0x0010L, /// 垂直方向の中央に表示。
			vertical = 0x0040L, /// 位置を調整するときに垂直方向を優先する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Flags);
	};

public:
	/// 空のポップアップメニューを作成。
	PopupMenu();
	PopupMenu(PopupMenu&& value);
	/// 子項目の情報配列から作成。
	PopupMenu(ArrayRange<const Menu::ItemInfo> itemInfos);
	virtual ~PopupMenu();
	PopupMenu& operator=(PopupMenu&& value);

public:
	/// ポップアップメニューを表示する直前のイベント。
	Listener<Menu::PopupBegin&>& onPopupBegin();
	/// ポップアップメニューを表示し終わったイベント。
	Listener<Menu::PopupEnd&>& onPopupEnd();
	/// オーナーコントロールとオーナーのクライアント座標を指定してポップアップメニューを表示する。
	void show(Control& owner, const Point& position, PopupMenu::Flags flags = Flags::bothButton | Flags::vertical);
	void show(Control& owner, int x, int y, PopupMenu::Flags flags = Flags::bothButton | Flags::vertical);
};



	}
}