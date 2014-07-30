#include "ComboBox.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Frame.hpp>
#include <balor/gui/ScrollBar.hpp>
#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using std::vector;
using namespace balor::graphics;
using namespace balor::system;


namespace {
static_assert(ComboBox::Style::simple       == CBS_SIMPLE, "Invalid enum value");
static_assert(ComboBox::Style::dropDown     == CBS_DROPDOWN, "Invalid enum value");
static_assert(ComboBox::Style::dropDownList == CBS_DROPDOWNLIST, "Invalid enum value");

static_assert(ComboBox::Options::noHScroll        == CBS_AUTOHSCROLL, "Invalid enum value");
static_assert(ComboBox::Options::noIntegralHeight == CBS_NOINTEGRALHEIGHT, "Invalid enum value");

const int optionsMask = CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT;
const int optionsXorMask = CBS_AUTOHSCROLL;
} // namespace



bool ComboBox::Style::_validate(Style value) {
	switch (value) {
		case simple       :
		case dropDown     :
		case dropDownList : return true;
		default           : return false;
	}
}


bool ComboBox::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



ComboBox::Edit::Edit() {
}


ComboBox::Edit::Edit(Edit&& value) : ::balor::gui::Edit(move(value), false) {
	assert("Object sliced" && typeid(*this) == typeid(value));
}


ComboBox::Edit::Edit(HWND handle) {
	attachHandle(handle);
}


ComboBox::Edit::~Edit() {
	if (_handle && IsWindow(_handle)) {
		_handle.procedure(_defaultProcedure);
	}
	_handle = Handle();
}


ComboBox::Edit& ComboBox::Edit::operator=(Edit&& value) {
	if (this != &value) {
		this->~Edit();
		new (this) Edit(move(value));
	}
	return *this;
}


void ComboBox::Edit::parent(Control* ) {
	assert("Can't change ComboBox::Edit's parent" && false);
}


bool ComboBox::Edit::isInputKey(int shortcut) const {
	int key = shortcut & (~Key::Modifier::mask | Key::Modifier::alt);
	if (key == Key::enter || key == Key::escape) { // ドロップダウン中の Enter, Esc キーはダイアログキーではない
		auto parent = dynamic_cast<ComboBox*>(this->parent());
		if (parent && parent->style() != Style::simple && parent->dropDowned()) {
			return true;
		}
	}
	return Control::isInputKey(shortcut);
}


void ComboBox::Edit::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_SETFOCUS : { // onActivate が発生しないようにする
			processMessageByDefault(msg);
			//Focus event(*this, (HWND)msg.wparam);
			//onFocus()(event);
			if (!_caretVisible) {
				verify(HideCaret(*this));
			}
			if (_caret != reinterpret_cast<HBITMAP>(0xffffffff)) {
				verify(CreateCaret(*this, _caret, _caretSize.width, _caretSize.height));
				verify(ShowCaret(*this));
			}
		} break;
		default : {
			::balor::gui::Edit::processMessage(msg);
		} break;
	}
}



ComboBox::List::List() {
}


ComboBox::List::List(List&& value) : ListBox(move(value), false) {
	assert("Object sliced" && typeid(*this) == typeid(value));
}


ComboBox::List::List(HWND handle) {
	attachHandle(handle);
}


ComboBox::List::~List() {
	if (_handle && IsWindow(_handle)) {
		_handle.procedure(_defaultProcedure);
	}
	_handle = Handle();
}


ComboBox::List& ComboBox::List::operator=(List&& value) {
	if (this != &value) {
		this->~List();
		new (this) List(move(value));
	}
	return *this;
}


void ComboBox::List::parent(Control* ) {
	assert("Can't change ComboBox::List's parent" && false);
}



ComboBox::ComboBox() {
}


ComboBox::ComboBox(ComboBox&& value, bool checkSlicing)
	: Control(move(value))
	, _edit(move(value._edit))
	, _list(move(value._list))
	, _onCloseUp(move(value._onCloseUp))
	, _onDropDown(move(value._onDropDown))
	, _onSelect(move(value._onSelect))
	, _onSelectCancel(move(value._onSelectCancel))
	, _onSelectEnd(move(value._onSelectEnd))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ComboBox::ComboBox(Control& parent, int x, int y, int width, int height, ComboBox::Style style, ComboBox::Options options) {
	assert("Invalid ConboBox::Style" && Style::_validate(style));
	assert("Invalid ConboBox::Options" && Options::_validate(options));
	assert("Can't set Options::noIntegralHeight when style is Style::dropDownList" && !(options & Options::noIntegralHeight && style == Style::dropDownList)); // この状態だとドロップダウンが表示されなくなる

	// CBS_DISABLENOSCROLL は非常に謎の挙動をするのでサポートから外した。MSDN では vertical scrollBar とあるが水平スクロールバーが固定される。
	// それに伴って高さが狭くなり、必ず垂直スクロールバーが表示される。style() が Style::simple の場合は垂直と水平スクロールバーが固定される。
	attachHandle(CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | CBS_HASSTRINGS | style | (options ^ optionsXorMask)
		, x, y, width, height
		, parent, nullptr, nullptr, nullptr));

	COMBOBOXINFO info;
	ZeroMemory(&info, sizeof(info));
	info.cbSize = sizeof(info);
	verify(SendMessageW(handle(), CB_GETCOMBOBOXINFO, 0, (LPARAM)&info));
	if (style != Style::dropDownList) {
		_edit = Edit(info.hwndItem);
	}
	_list = List(info.hwndList);
}


ComboBox::ComboBox(Control& parent, int x, int y, int width, int height, StringRangeArray items, ComboBox::Style style, ComboBox::Options options) {
	*this = ComboBox(parent, x, y, width, height, style, options);
	this->list().items(items);
	size(getPreferredSize(width, height));
}


ComboBox::~ComboBox() {
}


ComboBox& ComboBox::operator=(ComboBox&& value) {
	if (&value != this) {
		this->~ComboBox();
		new (this) ComboBox(move(value));
	}
	return *this;
}


Rectangle ComboBox::boundsWithDropDown() const {
	RECT rect;
	verify(SendMessageW(handle(), CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&rect));
	Rectangle bounds = rect;
	auto parent = this->parent();
	if (parent) {
		bounds.position(parent->screenToClient(bounds.position()));
	}
	return bounds;
}


Rectangle ComboBox::dropDownButtonBounds() const {
	COMBOBOXINFO info;
	info.cbSize = sizeof(info);
	verify(SendMessageW(handle(), CB_GETCOMBOBOXINFO, 0, (LPARAM)&info));
	return info.rcButton;
}


bool ComboBox::dropDownButtonIsPushed() const {
	COMBOBOXINFO info;
	info.cbSize = sizeof(info);
	verify(SendMessageW(handle(), CB_GETCOMBOBOXINFO, 0, (LPARAM)&info));
	return info.stateButton == STATE_SYSTEM_PRESSED;
}


bool ComboBox::dropDowned() const {
	return SendMessageW(handle(), CB_GETDROPPEDSTATE , 0, 0) == TRUE;
}


void ComboBox::dropDowned(bool value) {
	verify(SendMessageW(handle(), CB_SHOWDROPDOWN, value ? TRUE : FALSE, 0) == TRUE);
}


int ComboBox::dropDownScrollWidth() const {
	return SendMessageW(handle(), CB_GETHORIZONTALEXTENT, 0, 0);
}


void ComboBox::dropDownScrollWidth(int value) {
	if (value != dropDownScrollWidth()) {
		SendMessageW(handle(), CB_SETHORIZONTALEXTENT, value, 0);
	}
}


int ComboBox::dropDownWidth() const {
	if (style() == Style::simple) {
		return size().width;
	}
	int width = SendMessageW(handle(), CB_GETDROPPEDWIDTH, 0, 0);
	assert(0 <= width);
	return width;
}


void ComboBox::dropDownWidth(int value) {
	assert("Negative dropDownWidth" && 0 <= value);
	if (value != dropDownWidth()) {
		verify(0 <= SendMessageW(handle(), CB_SETDROPPEDWIDTH, value, 0));
	}
}


ComboBox::Edit& ComboBox::edit() {
	return _edit;
}


const ComboBox::Edit& ComboBox::edit() const {
	return _edit;
}


int ComboBox::editHeight() const {
	int height = SendMessageW(handle(), CB_GETITEMHEIGHT, (WPARAM)-1, 0);
	assert("Failed to CB_GETITEMHEIGHT" && height != CB_ERR);
	return height;
}


void ComboBox::editHeight(int value) {
	assert("Negative editHeight" && 0 <= value);
	assert("Can't change editHeight" && !(style() == Style::simple && !(options() & Options::noIntegralHeight)));
	verify(SendMessageW(handle(), CB_SETITEMHEIGHT, (WPARAM)-1, value) != CB_ERR);
}


bool ComboBox::extendedUI() const {
	return SendMessageW(handle(), CB_GETEXTENDEDUI, 0, 0) == TRUE;
}


void ComboBox::extendedUI(bool value) {
	verify(SendMessageW(handle(), CB_SETEXTENDEDUI, value ? TRUE : FALSE, 0) == CB_OKAY);
}


bool ComboBox::focused() const {
	HWND focusHandle = GetFocus();
	return focusHandle == handle() || IsChild(*this, focusHandle);
}


Size ComboBox::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	int newWidth = list().maxItemWidth();
	bool styleIsSimple = style() == Style::simple;
	if (!styleIsSimple) {
		newWidth += ScrollBar::defaultVerticalWidth();
	}
	auto borderSize = Size(GetSystemMetrics(SM_CXFIXEDFRAME), GetSystemMetrics(SM_CYFIXEDFRAME));
	auto border3dSize = Size(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));
	newWidth += borderSize.width * 3;

	int newHeight = font().height() + border3dSize.height + borderSize.height * 2;
	if (styleIsSimple) {
		newHeight += list().getPreferredSize(1, 0).height;
	}
	return Size(width  ? width  : newWidth 
			   ,height ? height : newHeight);
}


ComboBox::List& ComboBox::list() {
	return _list;
}


const ComboBox::List& ComboBox::list() const {
	return _list;
}


int ComboBox::maxVisibleItemCount() const {
	assert("This function needs visual style" && System::visualStyleEnabled());
#define CB_GETMINVISIBLE 0x1702 // どうして定義されていないのだろう？
	return SendMessageW(handle(), CB_GETMINVISIBLE, 0, 0);
}


void ComboBox::maxVisibleItemCount(int value) {
	assert("This function needs visual style" && System::visualStyleEnabled());
	assert("Invalid maxVisibleItemCount" && 0 < value);
	assert("Can't change maxVisibleItemCount" && !(options() & Options::noIntegralHeight));
	assert("Can't change maxVisibleItemCount" && style() != Style::simple);
#define CB_SETMINVISIBLE 0x1701 // どうして定義されていないのだろう？
	if (value != maxVisibleItemCount()) {
		verify(SendMessageW(handle(), CB_SETMINVISIBLE, value, 0));
	}
}


Listener<ComboBox::CloseUp&>& ComboBox::onCloseUp() { return _onCloseUp; }
Listener<ComboBox::DropDown&>& ComboBox::onDropDown() { return _onDropDown; }
Listener<ComboBox::Select&>& ComboBox::onSelect() { return _onSelect; }
Listener<ComboBox::SelectCancel&>& ComboBox::onSelectCancel() { return _onSelectCancel; }
Listener<ComboBox::SelectEnd&>& ComboBox::onSelectEnd() { return _onSelectEnd; }


ComboBox::Options ComboBox::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


int ComboBox::selectedIndex() const {
	return SendMessageW(handle(), CB_GETCURSEL, 0, 0);
}


void ComboBox::selectedIndex(int value) {
	assert("value out of range" && -1 <= value);
	assert("value out of range" && value < list().count());
	if (value != selectedIndex()) {
		verify(SendMessageW(handle(), CB_SETCURSEL, value, 0) == value);
	}
}


ComboBox::Style ComboBox::style() const {
	return static_cast<ComboBox::Style>(_handle.style() & (Style::simple | Style::dropDown | Style::dropDownList));
}


bool ComboBox::isInputKey(int shortcut) const {
	int key = shortcut & (~Key::Modifier::mask | Key::Modifier::alt);
	if (key == Key::enter || key == Key::escape) { // ドロップダウン中の Enter, Esc キーはダイアログキーではない
		if (dropDowned()) {
			return true;
		}
	}
	return Control::isInputKey(shortcut);
}


void ComboBox::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_COMMAND : {
			switch (HIWORD(msg.wparam)) {
				case CBN_CLOSEUP : {
					CloseUp event(*this);
					onCloseUp()(event);
				} break;
				case CBN_DROPDOWN : {
					DropDown event(*this);
					onDropDown()(event);
				} break;
				case CBN_KILLFOCUS : {
					Defocus event(*this, nullptr);
					onDefocus()(event);
				} break;
				case CBN_SELCHANGE : {
					Select event(*this);
					onSelect()(event);
				} break;
				case CBN_SELENDCANCEL : {
					SelectCancel event(*this);
					onSelectCancel()(event);
				} break;
				case CBN_SELENDOK  : {
					SelectEnd event(*this);
					onSelectEnd()(event);
				} break;
				case CBN_SETFOCUS : {
					auto frame = findFrame();
					if (frame) {
						frame->processDescendantFocused(*this);
					}
					Focus event(*this, (HWND)msg.wparam);
					onFocus()(event);
				} break;
			}
			Control::processMessage(msg);
		} break;
		case WM_KILLFOCUS :
		case WM_SETFOCUS : {
			processMessageByDefault(msg);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}