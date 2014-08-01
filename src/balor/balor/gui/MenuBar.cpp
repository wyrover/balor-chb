#include "MenuBar.hpp"

#include <utility>

#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>


namespace balor {
	namespace gui {


using std::move;



MenuBar::MenuBar() {
}


MenuBar::MenuBar(MenuBar&& value) : Menu(move(value)) {
}


MenuBar::MenuBar(ArrayRange<const Menu::ItemInfo> itemInfos) {
	this->itemInfos(itemInfos);
}


MenuBar::~MenuBar() {
}


MenuBar& MenuBar::operator=(MenuBar&& value) {
	if (&value != this) {
		Menu::operator=(move(value));
	}
	return *this;
}


void MenuBar::add(const ItemInfo& itemInfo) {
	if (!*this) {
		_attachHandle(CreateMenu());
	}
	Menu::add(itemInfo);
}


void MenuBar::insert(int index, const ItemInfo& itemInfo) {
	if (!*this) {
		_attachHandle(CreateMenu());
	}
	Menu::insert(index, itemInfo);
}


void MenuBar::itemInfos(ArrayRange<const Menu::ItemInfo> value) {
	if (!*this) {
		_attachHandle(CreateMenu());
	}
	Menu::itemInfos(value);
}



	}
}