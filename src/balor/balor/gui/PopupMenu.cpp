#include "PopupMenu.hpp"

#include <utility>

#include <balor/gui/Control.hpp>
#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>
#include <balor/Point.hpp>


namespace balor {
	namespace gui {


using std::move;

static_assert(PopupMenu::Flags::recursive  == TPM_RECURSE, "Invalid enum value");
static_assert(PopupMenu::Flags::bothButton == TPM_RIGHTBUTTON, "Invalid enum value");
static_assert(PopupMenu::Flags::hCentor    == TPM_CENTERALIGN, "Invalid enum value");
static_assert(PopupMenu::Flags::right      == TPM_RIGHTALIGN, "Invalid enum value");
static_assert(PopupMenu::Flags::bottom     == TPM_BOTTOMALIGN, "Invalid enum value");
static_assert(PopupMenu::Flags::vCentor    == TPM_VCENTERALIGN, "Invalid enum value");
static_assert(PopupMenu::Flags::vertical   == TPM_VERTICAL, "Invalid enum value");



PopupMenu::PopupMenu() {
}


PopupMenu::PopupMenu(PopupMenu&& value) : Menu(move(value)) {
}


PopupMenu::PopupMenu(ArrayRange<const Menu::ItemInfo> itemInfos) {
	this->itemInfos(itemInfos);
}


PopupMenu::~PopupMenu() {
}


PopupMenu& PopupMenu::operator=(PopupMenu&& value) {
	if (&value != this) {
		Menu::operator=(move(value));
	}
	return *this;
}


Listener<Menu::PopupBegin&>& PopupMenu::onPopupBegin() {
	return _root.onPopupBegin();
}


Listener<Menu::PopupEnd&>& PopupMenu::onPopupEnd() {
	return _root.onPopupEnd();
}


void PopupMenu::show(Control& owner, const Point& position, PopupMenu::Flags flags) {
	assert("Invalid popup menu owner" && owner && owner.visible());
	const Point screen = owner.clientToScreen(position);
	verify(TrackPopupMenu(*this, flags, screen.x, screen.y, 0, owner.handle(), nullptr));
}


void PopupMenu::show(Control& owner, int x, int y, PopupMenu::Flags flags) {
	show(owner, Point(x, y), flags);
}



	}
}