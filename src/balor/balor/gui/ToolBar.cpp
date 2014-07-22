#include "ToolBar.hpp"

#include <algorithm>
#include <utility>

//#include <balor/graphics/Bitmap.hpp>
//#include <balor/graphics/Brush.hpp>
//#include <balor/graphics/Color.hpp>
//#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/ImageList.hpp>
#include <balor/gui/ToolTip.hpp>
#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/StringBuffer.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using std::swap;
using std::vector;
using namespace balor::graphics;
using namespace balor::system;


namespace {
static_assert(ToolBar::ItemStyle::button         == BTNS_BUTTON, "Invalid enum value");
static_assert(ToolBar::ItemStyle::separator      == BTNS_SEP, "Invalid enum value");
static_assert(ToolBar::ItemStyle::checkBox       == BTNS_CHECK, "Invalid enum value");
static_assert(ToolBar::ItemStyle::radioButton    == BTNS_CHECKGROUP, "Invalid enum value");
static_assert(ToolBar::ItemStyle::dropDown       == (BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN), "Invalid enum value");
static_assert(ToolBar::ItemStyle::dropDownButton == BTNS_DROPDOWN, "Invalid enum value");

const int itemStyleMask = BTNS_BUTTON | BTNS_SEP | BTNS_CHECK | BTNS_CHECKGROUP | BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN;

int getItemCount(HWND handle) {
	return SendMessageW(handle, TB_BUTTONCOUNT, 0, 0);
}


int imageIndexToiImage(int imageIndex) {
	return imageIndex == -1 ? I_IMAGENONE : imageIndex;
}


int iImageToImageIndex(int iImage) {
	return iImage == I_IMAGENONE ? -1 : iImage;
}


struct ItemNewData : public TBBUTTON {
	ItemNewData() {}
	ItemNewData(int index, const ToolBar::ItemInfo& info, int style, int state) {
		this->iBitmap = imageIndexToiImage(info.imageIndex());
		this->fsStyle = static_cast<BYTE>(style);
		this->fsState = static_cast<BYTE>(state);
		this->idCommand = index;
		this->iString = info.text().empty() ? 0 : reinterpret_cast<INT_PTR>(info.text().c_str());
	}
};


struct ItemData : public TBBUTTONINFOW {
	ItemData() {
		this->cbSize = sizeof(TBBUTTONINFOW);
	}
	ItemData(UINT mask) {
		this->cbSize = sizeof(TBBUTTONINFOW);
		this->dwMask = mask;
	}
	ItemData(int index, const ToolBar::ItemInfo& info, int style, int state) {
		this->cbSize = sizeof(TBBUTTONINFOW);
		this->dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE | TBIF_TEXT;
		this->iImage = imageIndexToiImage(info.imageIndex());
		this->fsStyle = static_cast<BYTE>(style);
		this->fsState = static_cast<BYTE>(state);
		this->idCommand = index;
		this->pszText = const_cast<LPWSTR>(info.text().empty() ? nullptr : info.text().c_str());
	}

	void getFrom(const ToolBar::Item& item) {
		verify(SendMessageW(item.ownerHandle(), TB_GETBUTTONINFOW, item.index(), (LPARAM)this) == item.index());
	}
	void setTo(ToolBar::Item&& item) {
		setTo(item);
	}
	void setTo(ToolBar::Item& item) {
		verify(SendMessageW(item.ownerHandle(), TB_SETBUTTONINFOW, item.index(), (LPARAM)this));
	}
	int getStyle(const ToolBar::Item& item, UINT mask) {
		this->dwMask = TBIF_STYLE;
		verify(SendMessageW(item.ownerHandle(), TB_GETBUTTONINFOW, item.index(), (LPARAM)this) == item.index());
		return this->fsStyle & mask;
	}
	void setStyle(const ToolBar::Item& item, UINT mask, UINT style) {
		this->dwMask = TBIF_STYLE;
		verify(SendMessageW(item.ownerHandle(), TB_GETBUTTONINFOW, item.index(), (LPARAM)this) == item.index());
		style = (this->fsStyle & ~mask) | style;
		if (this->fsStyle != style) {
			this->fsStyle = static_cast<BYTE>(style);
			verify(SendMessageW(item.ownerHandle(), TB_SETBUTTONINFOW, item.index(), (LPARAM)this));
		}
	}
};


bool getExtendedStyle(HWND handle, int flag) {
	return toFlag(SendMessageW(handle, TB_GETEXTENDEDSTYLE, 0, 0))[flag];
}


void setExtendedStyle(HWND handle, int flag, bool value) {
	auto style = toFlag(SendMessageW(handle, TB_GETEXTENDEDSTYLE, 0, 0));
	if (style[flag] != value) {
		style.set(flag, value);
		SendMessageW(handle, TB_SETEXTENDEDSTYLE, 0, style);
	}
}


int commandToIndex(HWND handle, int command) {
	int index = SendMessageW(handle, TB_COMMANDTOINDEX, command, 0);
	assert("Failed to TB_COMMANDTOINDEX" && index != -1);
	return index;
}
} // namespace



bool ToolBar::ItemStyle::_validate(ItemStyle value) {
	return (value & ~itemStyleMask) == 0;
}



ToolBar::Item::Item(HWND ownerHandle, int index) : _ownerHandle(ownerHandle), _index(index) {
	assert("Invalid toolBar handle" && IsWindow(ownerHandle));
	assert("item index out of range" && 0 <= index);
	assert("item index out of range" && commandToIndex(ownerHandle, index) < getItemCount(ownerHandle));
}


ToolBar::Item& ToolBar::Item::operator=(const ItemInfo& itemInfo) {
	ItemData data(index(), itemInfo, itemInfo._itemDataStyle, itemInfo._itemDataState);
	data.setTo(*this);
	onItemTipPopup() = itemInfo.onItemTipPopup();
	onClick() = itemInfo.onClick();
	return *this;
}


bool ToolBar::Item::autoSize() const {
	return ItemData().getStyle(*this, BTNS_AUTOSIZE) == BTNS_AUTOSIZE;
}


void ToolBar::Item::autoSize(bool value) {
	ItemData().setStyle(*this, BTNS_AUTOSIZE, value ? BTNS_AUTOSIZE : 0);
	verify(InvalidateRect(ownerHandle(), nullptr, TRUE));
}


Rectangle ToolBar::Item::bounds() const {
	if (!visible()) {
		return Rectangle(0, 0, 0, 0);
	}
	RECT rect;
	verify(SendMessageW(ownerHandle(), TB_GETITEMRECT, index(), (LPARAM)&rect));
	return rect;
}


bool ToolBar::Item::checked() const {
	return SendMessageW(ownerHandle(), TB_ISBUTTONCHECKED, index(), 0) != 0;
}


void ToolBar::Item::checked(bool value) {
	verify(SendMessageW(ownerHandle(), TB_CHECKBUTTON, index(), value ? TRUE : FALSE));
}


bool ToolBar::Item::enabled() const {
	return SendMessageW(ownerHandle(), TB_ISBUTTONENABLED, index(), 0) != 0;
}


void ToolBar::Item::enabled(bool value) {
	verify(SendMessageW(ownerHandle(), TB_ENABLEBUTTON, index(), value ? TRUE : FALSE));
}


int ToolBar::Item::imageIndex() const {
	return iImageToImageIndex((SHORT)SendMessageW(ownerHandle(), TB_GETBITMAP, index(), 0));
}


void ToolBar::Item::imageIndex(int value) {
	assert("Invalid imageIndex" && -1 <= value);
	verify(SendMessageW(ownerHandle(), TB_CHANGEBITMAP, index(), imageIndexToiImage(value)));
}


int ToolBar::Item::index() const {
	return _index;
}


bool ToolBar::Item::noPrefix() const {
	return ItemData().getStyle(*this, BTNS_NOPREFIX) == BTNS_NOPREFIX;
}


void ToolBar::Item::noPrefix(bool value) {
	ItemData().setStyle(*this, BTNS_NOPREFIX, value ? BTNS_NOPREFIX : 0);
}


Listener<ToolBar::Click&>& ToolBar::Item::onClick() {
	assert("Invalid owner handle" && dynamic_cast<ToolBar*>(fromHandle(ownerHandle())));
	ToolBar* owner = static_cast<ToolBar*>(fromHandle(ownerHandle()));
	return owner->_itemListeners[index()].onClick;
}


Listener<ToolBar::ItemTipPopup&>& ToolBar::Item::onItemTipPopup() {
	assert("Invalid owner handle" && dynamic_cast<ToolBar*>(fromHandle(ownerHandle())));
	ToolBar* owner = static_cast<ToolBar*>(fromHandle(ownerHandle()));
	return owner->_itemListeners[index()].onItemTipPopup;
}


HWND ToolBar::Item::ownerHandle() const {
	return _ownerHandle;
}


bool ToolBar::Item::rightText() const {
	return ItemData().getStyle(*this, BTNS_SHOWTEXT) == BTNS_SHOWTEXT;
}


void ToolBar::Item::rightText(bool value) {
	ItemData().setStyle(*this, BTNS_SHOWTEXT, value ? BTNS_SHOWTEXT : 0);
}


bool ToolBar::Item::pushed() const {
	return SendMessageW(ownerHandle(), TB_ISBUTTONPRESSED, index(), 0) != 0;
}


int ToolBar::Item::separatorSize() const {
	return imageIndex();
}


void ToolBar::Item::separatorSize(int value) {
	imageIndex(value);
}


ToolBar::ItemStyle ToolBar::Item::style() const {
	return static_cast<ItemStyle>(ItemData().getStyle(*this, itemStyleMask));
}


void ToolBar::Item::style(ToolBar::ItemStyle value) {
	assert("Invalid ToolBar::ItemStyle" && ItemStyle::_validate(value));
	ItemData().setStyle(*this, itemStyleMask, value);
}


String ToolBar::Item::text() const {
	wchar_t buffer[1024];
	buffer[0] = L'\0';
	ItemData data(TBIF_TEXT);
	data.pszText = buffer;
	data.cchText = sizeof(buffer) / sizeof(buffer[0]);
	data.getFrom(*this);
	return buffer;
}


void ToolBar::Item::text(StringRange value) {
	ItemData data(TBIF_TEXT);
	data.pszText = const_cast<wchar_t*>(value.c_str());
	data.setTo(*this);
}


bool ToolBar::Item::visible() const {
	return SendMessageW(ownerHandle(), TB_ISBUTTONHIDDEN, index(), 0) == 0;
}


void ToolBar::Item::visible(bool value) {
	verify(SendMessageW(ownerHandle(), TB_HIDEBUTTON, index(), value ? FALSE : TRUE));
}


int ToolBar::Item::width() const {
	ItemData data(TBIF_SIZE);
	data.getFrom(*this);
	return data.cx;
}


void ToolBar::Item::width(int value) {
	ItemData data(TBIF_SIZE);
	data.cx = static_cast<WORD>(value);
	data.setTo(*this);
}


bool ToolBar::Item::wrap() const {
	auto state = SendMessageW(ownerHandle(), TB_GETSTATE, index(), 0);
	assert(state != -1);
	return (state & TBSTATE_WRAP) != 0;
}


void ToolBar::Item::wrap(bool value) {
	auto state = SendMessageW(ownerHandle(), TB_GETSTATE, index(), 0);
	assert(state != -1);
	state = toFlag(state).set(TBSTATE_WRAP, value);
	verify(SendMessageW(ownerHandle(), TB_SETSTATE, index(), state));
	verify(InvalidateRect(ownerHandle(), nullptr, TRUE));
}



ToolBar::ItemInfo::ItemInfo()
	: _imageIndex(0)
	, _itemDataStyle(ItemStyle::separator)
	, _itemDataState(TBSTATE_ENABLED)
	{
}


ToolBar::ItemInfo::ItemInfo(ItemInfo&& value)
	: _imageIndex(move(value._imageIndex))
	, _itemDataStyle(move(value._itemDataStyle))
	, _text(move(value._text))
	, _itemDataState(move(value._itemDataState))
	, _onClick(move(value._onClick))
	, _onItemTipPopup(move(value._onItemTipPopup))
	{
}


ToolBar::ItemInfo::ItemInfo(const Item& item) {
	ItemData data(TBIF_IMAGE | TBIF_STATE | TBIF_STYLE);
	data.getFrom(item);

	_imageIndex = iImageToImageIndex(data.iImage);
	_itemDataStyle = data.fsStyle;
	_itemDataState = data.fsState;
	_onClick = const_cast<Item&>(item).onClick();
	_onItemTipPopup = const_cast<Item&>(item).onItemTipPopup();
	_text = item.text();
}


ToolBar::ItemInfo::ItemInfo(int imageIndex, String text, Listener<ToolBar::Click&> onClick)
	: _imageIndex(imageIndex)
	, _text(move(text))
	, _itemDataStyle(ItemStyle::button)
	, _itemDataState(TBSTATE_ENABLED)
	, _onClick(move(onClick)) {
	assert("Invalid imageIndex" && -1 <= imageIndex);
}


ToolBar::ItemInfo::ItemInfo(int imageIndex, String text, ToolBar::ItemStyle style, Listener<ToolBar::Click&> onClick)
	: _imageIndex(imageIndex)
	, _text(move(text))
	, _itemDataStyle(style)
	, _itemDataState(TBSTATE_ENABLED)
	, _onClick(move(onClick)) {
	assert("Invalid imageIndex" && -1 <= imageIndex);
	assert("Invalid ToolBar::ItemStyle" && ItemStyle::_validate(style));
}


ToolBar::ItemInfo& ToolBar::ItemInfo::operator=(ItemInfo&& value) {
	if (this != &value) {
		_imageIndex = move(value._imageIndex);
		_text = move(value._text);
		_itemDataStyle = move(value._itemDataStyle);
		_itemDataState = move(value._itemDataState);
		_onClick = move(value._onClick);
		_onItemTipPopup = move(value._onItemTipPopup);
	}
	return *this;
}


bool ToolBar::ItemInfo::autoSize() const { return toFlag(_itemDataStyle)[BTNS_AUTOSIZE]; }
void ToolBar::ItemInfo::autoSize(bool value) { _itemDataStyle = toFlag(_itemDataStyle).set(BTNS_AUTOSIZE, value); }
bool ToolBar::ItemInfo::checked() const { return toFlag(_itemDataState)[TBSTATE_CHECKED]; }
void ToolBar::ItemInfo::checked(bool value) { _itemDataState = toFlag(_itemDataState).set(TBSTATE_CHECKED, value); }
bool ToolBar::ItemInfo::enabled() const { return toFlag(_itemDataState)[TBSTATE_ENABLED]; }
void ToolBar::ItemInfo::enabled(bool value) { _itemDataState = toFlag(_itemDataState).set(TBSTATE_ENABLED, value); }
int ToolBar::ItemInfo::imageIndex() const { return _imageIndex; }
void ToolBar::ItemInfo::imageIndex(int value) {
	assert("Invalid imageIndex" && -1 <= value);
	_imageIndex = value;
}
bool ToolBar::ItemInfo::noPrefix() const { return toFlag(_itemDataStyle)[BTNS_NOPREFIX]; }
void ToolBar::ItemInfo::noPrefix(bool value) { _itemDataStyle = toFlag(_itemDataStyle).set(BTNS_NOPREFIX, value); }
Listener<ToolBar::Click&>& ToolBar::ItemInfo::onClick() { return _onClick; }
const Listener<ToolBar::Click&>& ToolBar::ItemInfo::onClick() const { return _onClick; }
Listener<ToolBar::ItemTipPopup&>& ToolBar::ItemInfo::onItemTipPopup() { return _onItemTipPopup; }
const Listener<ToolBar::ItemTipPopup&>& ToolBar::ItemInfo::onItemTipPopup() const { return _onItemTipPopup; }
bool ToolBar::ItemInfo::rightText() const { return toFlag(_itemDataStyle)[BTNS_SHOWTEXT]; }
void ToolBar::ItemInfo::rightText(bool value) { _itemDataStyle = toFlag(_itemDataStyle).set(BTNS_SHOWTEXT, value); }
int ToolBar::ItemInfo::separatorSize() const { return imageIndex(); }
void ToolBar::ItemInfo::separatorSize(int value) { imageIndex(value); }
ToolBar::ItemStyle ToolBar::ItemInfo::style() const { return static_cast<ItemStyle>(_itemDataStyle & itemStyleMask); }
void ToolBar::ItemInfo::style(ToolBar::ItemStyle value) {
	assert("Invalid ToolBar::ItemStyle" && ItemStyle::_validate(value));
	_itemDataStyle = (_itemDataStyle & ~itemStyleMask) | value;
}
const String& ToolBar::ItemInfo::text() const { return _text; }
void ToolBar::ItemInfo::text(String value) { _text = move(value); }
bool ToolBar::ItemInfo::visible() const { return !toFlag(_itemDataState)[TBSTATE_HIDDEN]; }
void ToolBar::ItemInfo::visible(bool value) { _itemDataState = toFlag(_itemDataState).set(TBSTATE_HIDDEN, !value); }
bool ToolBar::ItemInfo::wrap() const { return toFlag(_itemDataState)[TBSTATE_WRAP]; }
void ToolBar::ItemInfo::wrap(bool value) { _itemDataState = toFlag(_itemDataState).set(TBSTATE_WRAP, value); }



ToolBar::ItemEvent::ItemEvent(ToolBar& sender, int itemIndex)
	: ToolBar::Event(sender), _itemIndex(itemIndex) {
}


ToolBar::Item ToolBar::ItemEvent::item() const { return Item(sender(), _itemIndex); }



ToolBar::Click::Click(ToolBar& sender, int itemIndex, bool dropDown)
	: ItemEvent(sender, itemIndex), _dropDown(dropDown) {
}


bool ToolBar::Click::dropDown() const { return _dropDown; }



ToolBar::ItemTipPopup::ItemTipPopup(ToolBar& sender, int itemIndex, wchar_t* buffer, int bufferSize)
	: ItemEvent(sender, itemIndex), _buffer(buffer), _bufferSize(bufferSize) {
}


void ToolBar::ItemTipPopup::setText(StringRange value) {
	int length = min(value.length(), _bufferSize - 1);
	String::refer(value).copyTo(0, _buffer, _bufferSize, 0, length);
	_buffer[length] = L'\0';
}



ToolBar::ToolBar() {
}


ToolBar::ToolBar(ToolBar&& value, bool checkSlicing)
	: Control(move(value))
	, _itemListeners(move(value._itemListeners))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ToolBar::ToolBar(Control& parent, int x, int y, int width, int height, HIMAGELIST imageList) {
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_BAR_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(WS_EX_CONTROLPARENT, TOOLBARCLASSNAMEW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		| CCS_NOPARENTALIGN | CCS_NORESIZE | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | CCS_NODIVIDER
		, x, y, width ? width : 0, height ? height : 0
		, parent, nullptr,nullptr, nullptr));

	// Visual style 有効だと見た目は最初からフラットだが設定しないとセパレータが表示されない、等の不具合あり。
	// CreateWindowExW に直接指定すると背景やボタンの描画がおかしくなる。
	_handle.setStyle(TBSTYLE_FLAT, true);

	setExtendedStyle(handle(), TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS, true);
	SendMessageW(handle(), TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessageW(handle(), TB_SETIMAGELIST, 0, (LPARAM)imageList);
	size(getPreferredSize(width, height));
}


ToolBar::ToolBar(Control& parent, int x, int y, int width, int height, HIMAGELIST imageList, ArrayRange<const ItemInfo> itemInfos) {
	*this = ToolBar(parent, x, y, width, height, imageList);
	this->itemInfos(itemInfos);
	size(getPreferredSize(width, height));
}


ToolBar::~ToolBar() {
}


ToolBar& ToolBar::operator=(ToolBar&& value) {
	if (this != &value) {
		this->~ToolBar();
		new (this) ToolBar(move(value));
	}
	return *this;
}


void ToolBar::add(const ItemInfo& itemInfo) {
	ItemNewData data(count(), itemInfo, itemInfo._itemDataStyle, itemInfo._itemDataState);
	verify(SendMessageW(handle(), TB_ADDBUTTONSW, 1, (LPARAM)&data));
	_itemListeners.push_back(ItemListener(itemInfo));
}


bool ToolBar::bottomText() const {
	return !_handle.hasStyle(TBSTYLE_LIST);
}


void ToolBar::bottomText(bool value) {
	auto infos = itemInfos();
	_handle.setStyle(TBSTYLE_LIST, !value);
	itemInfos(infos); // 項目を再設定しないとツールチップ等がおかしくなる。
}


void ToolBar::clear() {
	for (int i = count(); 0 <= --i; ) {
		erase(i);
	}
}


int ToolBar::count() const {
	return SendMessageW(handle(), TB_BUTTONCOUNT, 0, 0);
}


ImageList ToolBar::disabledImageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), TB_GETDISABLEDIMAGELIST, 0, 0));
}


void ToolBar::disabledImageList(HIMAGELIST value) {
	if (value != disabledImageList()) {
		SendMessageW(handle(), TB_SETDISABLEDIMAGELIST, 0, (LPARAM)value);
		invalidate();
	}
}


bool ToolBar::divider() const {
	return !_handle.hasStyle(CCS_NODIVIDER);
}


void ToolBar::divider(bool value) {
	_handle.setStyle(CCS_NODIVIDER, !value);
}


Control::Edge ToolBar::edge() const {
	return _handle.edge();
}


void ToolBar::edge(Control::Edge value) {
	_handle.edge(value);
}


void ToolBar::erase(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	verify(SendMessageW(handle(), TB_DELETEBUTTON, index, 0));
	ItemData data(TBIF_COMMAND);
	for (int i = count(); index < i; --i) {
		data.idCommand = i - 1;
		data.setTo(Item(handle(), i));
	}
	_itemListeners.erase(_itemListeners.begin() + index);
}


int ToolBar::focusedIndex() const {
	return SendMessageW(handle(), TB_GETHOTITEM, 0, 0);
}


void ToolBar::focusedIndex(int value) {
	assert("index out of range" && -1 <= value);
	assert("index out of range" && value < count());
	if (value != focusedIndex()) {
		SendMessageW(handle(), TB_SETHOTITEM, value, 0);
	}
}


int ToolBar::getIndexAt(const Point& point) const {
	POINT winPoint = point;
	return SendMessageW(handle(), TB_HITTEST, 0, (LPARAM)&winPoint);
}


int ToolBar::getIndexAt(int x, int y) const {
	return getIndexAt(Point(x, y));
}


Size ToolBar::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	Size newSize = itemsSize();

	newSize.height += 4;
	if (divider()) {
		newSize.height += 2;
	}
	auto size = sizeFromClientSize(newSize);
	newSize.width += (size.width - newSize.width) / 2; // width は座標から求めているので左のエッジが含まれている。
	newSize.height = size.height;
	auto parent = this->parent();
	if (parent) {
		if (vertical()) {
			newSize.height = parent->clientSize().height;
		} else {
			newSize.width  = parent->clientSize().width;
		}
	}
	return Size(width   ? width  : newSize.width 
				,height ? height : newSize.height);
}


bool ToolBar::hideClippedItems() const {
	return getExtendedStyle(handle(), TBSTYLE_EX_HIDECLIPPEDBUTTONS);
}


void ToolBar::hideClippedItems(bool value) {
	setExtendedStyle(handle(), TBSTYLE_EX_HIDECLIPPEDBUTTONS, value);
}


ImageList ToolBar::hotImageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), TB_GETHOTIMAGELIST, 0, 0));
}


void ToolBar::hotImageList(HIMAGELIST value) {
	if (value != hotImageList()) {
		SendMessageW(handle(), TB_SETHOTIMAGELIST, 0, (LPARAM)value);
		invalidate();
	}
}


ImageList ToolBar::imageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), TB_GETIMAGELIST, 0, 0));
}


void ToolBar::imageList(HIMAGELIST value) {
	if (value != imageList()) {
		SendMessageW(handle(), TB_SETIMAGELIST, 0, (LPARAM)value);
		invalidate();
	}
}


void ToolBar::indent(int value) {
	verify(SendMessageW(handle(), TB_SETINDENT, value, 0));
}


void ToolBar::insert(int index, const ItemInfo& itemInfo) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= count());
	int count = this->count();
	ItemData data(TBIF_COMMAND);
	for (int i = count; index <= --i; ) { // command で項目を識別するので command を後ろから１ずつずらしていく。前からだと最初の項目しか更新されない。
		data.idCommand = i + 1;
		data.setTo(Item(handle(), i));
	}
	ItemNewData newData(index, itemInfo, itemInfo._itemDataStyle, itemInfo._itemDataState);
	verify(SendMessageW(handle(), TB_INSERTBUTTON, index, (LPARAM)&newData));
	_itemListeners.insert(_itemListeners.begin() + index, ItemListener(itemInfo));
}


vector<ToolBar::ItemInfo> ToolBar::itemInfos() const {
	vector<ItemInfo> infos;
	int count = this->count();
	infos.reserve(count);
	for (int i = 0; i < count; ++i) {
		infos.push_back(Item(handle(), i));
	}
	return infos;
}


void ToolBar::itemInfos(ArrayRange<const ToolBar::ItemInfo> value) {
	clear();
	for (int i = 0, end = value.size(); i < end; ++i) {
		add(value[i]);
	}
}


Size ToolBar::itemsSize() const {
	Size size(0, 0);
	int lastVisibleIndex = -1;
	for (int i = 0, end = count(); i != end; ++i) {
		if ((*this)[i].visible()) {
			lastVisibleIndex = i;
		}
	}
	if (0 <= lastVisibleIndex) {
		auto lastVisible = (*this)[lastVisibleIndex];
		auto lastBounds = lastVisible.bounds();
		size.width  = (lastVisible.style() == ItemStyle::separator && lastVisible.wrap()) ? lastBounds.left() : lastBounds.right();
		size.height = lastBounds.height;
		for (int i = 0; i < lastVisibleIndex; ++i) {
			auto item = (*this)[i]; // セパレータで改行する場合、自分の幅のぶんだけ縦にもスペースを空けるので考慮する。
			if (item.visible() && item.wrap()) {
				auto bounds = item.bounds();
				size.width  = max(size.width, (item.style() == ItemStyle::separator) ? bounds.left() : bounds.right());
				size.height += bounds.height;
			}
		}
	}
	return size;
}


ToolTip ToolBar::itemTip() {
	return ToolTip((HWND)SendMessageW(handle(), TB_GETTOOLTIPS, 0, 0));
}


Size ToolBar::sizeFromClientSize(const Size& clientSize) const {
	auto size = Control::sizeFromClientSize(clientSize);
	if (divider()) {
		size.height += 2;
	}
	return size;
}


bool ToolBar::transparent() const {
	return _handle.hasStyle(TBSTYLE_TRANSPARENT);
}


void ToolBar::transparent(bool value) {
	_handle.setStyle(TBSTYLE_TRANSPARENT, value);
}


bool ToolBar::vertical() const {
	return _handle.hasStyle(CCS_VERT);
}


void ToolBar::vertical(bool value) {
	_handle.setStyle(CCS_VERT, value);
	for (int i = 0, end = count(); i < end; ++i) {
		(*this)[i].wrap(value);
	}
}


bool ToolBar::wrappable() const {
	return _handle.hasStyle(TBSTYLE_WRAPABLE);
}


void ToolBar::wrappable(bool value) {
	assert("Can't set wrappable and vertical true" && !(value && vertical()));
	if (value != wrappable()) {
		_handle.setStyle(TBSTYLE_WRAPABLE, value);
		if (!value) { // 改行状態をクリアしておく。
			for (int i = 0, end = count(); i < end; ++i) {
				(*this)[i].wrap(false);
			}
		}
	}
}


ToolBar::Item ToolBar::operator[](int index) {
	return Item(handle(), index);
}


const ToolBar::Item ToolBar::operator[](int index) const {
	return Item(handle(), index);
}


void ToolBar::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_COMMAND : {
			int index = LOWORD(msg.wparam);
			Click event(*this, index, false);
			_itemListeners[index].onClick(event);
			Control::processMessage(msg);
		} break;
		case wmReflect + WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				//case NM_CUSTOMDRAW : {
				//	auto info = (NMTBCUSTOMDRAW*)msg.lparam;
				//	if (info->nmcd.dwDrawStage == CDDS_PREPAINT) {
				//		msg.result = CDRF_NOTIFYITEMDRAW;
				//	} else if (info->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
				//	// Visual Style が有効な場合は全て無効な模様。
				//		info->clrText = Color::red().toCOLORREF();
				//		info->clrBtnFace = Color::red().toCOLORREF();
				//		info->clrBtnHighlight = Color::red().toCOLORREF();
				//		info->clrHighlightHotTrack = Color::red().toCOLORREF();
				//		info->hbrLines = Brush::white();
				//		info->hbrMonoDither = Brush::white();
				//		info->hpenLines = Pen::white();
				//	}
				//	return;
				//} break;
				case TBN_DROPDOWN : {
					auto info = (NMTOOLBARW*)msg.lparam;
					Click event(*this, info->iItem, true);
					_itemListeners[info->iItem].onClick(event);
					//msg.result = TBDDRET_TREATPRESSED; // 設定してもしなくても押されたように表示されている。
				} break;
				case TBN_GETINFOTIPW : {
					auto info = (NMTBGETINFOTIPW*)msg.lparam;
					ItemTipPopup event(*this, info->iItem, info->pszText, info->cchTextMax);
					_itemListeners[info->iItem].onItemTipPopup(event);
				} break;
				//case TBN_QUERYINSERT : {
				//	msg.result = 1;
				//	msg.handled = true;
				//} break;
				//case TBN_QUERYDELETE : {
				//	msg.result = 1;
				//	msg.handled = true;
				//} break;
			}
			Control::processMessage(msg);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



ToolBar::ItemListener::ItemListener() {
}


ToolBar::ItemListener::ItemListener(const ItemInfo& info)
	: onClick(info.onClick()), onItemTipPopup(info.onItemTipPopup()) {
}



	}
}