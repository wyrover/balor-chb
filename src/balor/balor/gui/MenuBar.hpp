#pragma once

#include <balor/gui/Menu.hpp>


namespace balor {
	namespace gui {



/**
 * メニューバー。
 *
 * メニューバーに常に表示されている最上位の項目の変更を反映するには Frame::updateMenuBar() を使うこと。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"MenuBar Sample");

	typedef MenuBar::ItemInfo Item;
	Item fileMenuItems[] = {
		Item(L"開く(&O)\tCtrl+O", Key::Modifier::ctrl | Key::o),
		Item(L"保存(&S)\tCtrl+S", Key::Modifier::ctrl | Key::s),
		Item(),
		Item(L"終了(&X)\tAlt+F4", Key::Modifier::alt | Key::f4)
	};
	auto onSubMenuClick = [&] (Menu::Click& e) {
		e.sender().checked(!e.sender().checked());
	};
	Item subMenuItems[] = {
		Item(L"チェック0", onSubMenuClick),
		Item(L"チェック1", onSubMenuClick),
		Item(L"チェック2", onSubMenuClick)
	};
	Item editMenuItems[] = {
		Item(L"サブメニュー(&S)", subMenuItems),
		Item(L"メニューを非表示(&H)", [&] (Menu::Click& ) {
			frame.menuBar(nullptr);
		})
	};
	Item menuItems[] = {
		Item(L"ファイル(&F)", fileMenuItems),
		Item(L"編集(&E)", editMenuItems)
	};
	MenuBar menuBar(menuItems);
	frame.menuBar(&menuBar);

	frame.onShortcutKey() = [&] (Frame::ShortcutKey& e) {
		switch (e.shortcut()) {
			case Key::Modifier::ctrl | Key::o : {
				MsgBox::show(L"開くメニューを選択");
				e.handled(true);
			} break;
			case Key::Modifier::ctrl | Key::s : {
				MsgBox::show(L"保存メニューを選択");
				e.handled(true);
			} break;
			case Key::Modifier::alt | Key::f4 : {
				frame.close();
				e.handled(true);
			} break;
		}
	};
 
	frame.runMessageLoop();
 * </code></pre>
 */
class MenuBar : public Menu {
public:
	/// 空のメニューバーを作成。
	MenuBar();
	MenuBar(MenuBar&& value);
	/// 子項目の情報配列から作成。
	MenuBar(ArrayRange<const Menu::ItemInfo> itemInfos);
	virtual ~MenuBar();
	MenuBar& operator=(MenuBar&& value);

public:
	/// 子項目を追加する。
	virtual void add(const ItemInfo& itemInfo);
	/// 子項目を挿入する。
	virtual void insert(int index, const ItemInfo& itemInfo);
	/// 子項目の情報配列を一括で設定する。
	using Menu::itemInfos;
	virtual void itemInfos(ArrayRange<const Menu::ItemInfo> value);
};



	}
}