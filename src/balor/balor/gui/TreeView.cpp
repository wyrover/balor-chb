#include "TreeView.hpp"

#include <algorithm>
#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/ImageList.hpp>
#include <balor/gui/Edit.hpp>
#include <balor/gui/ToolTip.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/StringBuffer.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <WindowsX.h>


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using std::swap;
using std::vector;
using namespace balor::graphics;


namespace {
static_assert(TreeView::Options::checkBoxes == TVS_CHECKBOXES, "Invalid enum value");

const int optionsMask = TVS_CHECKBOXES;
const int optionsXorMask = 0;


int stateToItemDataState(int state) {
	return INDEXTOSTATEIMAGEMASK(state + 1);
}


int itemDataStateToState(int itemDataState) {
	return ((itemDataState & TVIS_STATEIMAGEMASK) >> 12) - 1;
}


struct ItemData : public TVITEMW {
	ItemData() {}
	ItemData(HTREEITEM item, UINT mask) {
		this->hItem = item;
		this->mask = mask;
	}
	ItemData(HTREEITEM item, const TreeView::ItemInfo& info, int state) {
		this->hItem = item;
		this->mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_PARAM;
		this->pszText = const_cast<wchar_t*>(info.text().c_str());
		this->iImage = info.imageIndex();
		this->iSelectedImage = info.selectedImageIndex() == -1 ? info.imageIndex() : info.selectedImageIndex();
		this->state = state;
		this->stateMask = TVIS_CUT | TVIS_DROPHILITED | TVIS_BOLD | TVIS_STATEIMAGEMASK;
		this->lParam = 0;
	}

	void getFrom(HWND ownerHandle) {
		verify(SendMessageW(ownerHandle, TVM_GETITEMW, 0, (LPARAM)this));
	}
	void setTo(HWND ownerHandle) {
		verify(SendMessageW(ownerHandle, TVM_SETITEMW, 0, (LPARAM)this));
	}
	int getState(const TreeView::Item& item, UINT mask) {
		return SendMessageW(item.ownerHandle(), TVM_GETITEMSTATE, (WPARAM)(HTREEITEM)item, mask) & mask;
	}
	void setState(TreeView::Item& item, UINT mask, UINT value) {
		this->hItem = item;
		this->mask = TVIF_STATE;
		this->state = value;
		this->stateMask = mask;
		setTo(item.ownerHandle());
	}
};


HTREEITEM getNextItem(const TreeView::Item& item, int flag) {
	return reinterpret_cast<HTREEITEM>(SendMessageW(item.ownerHandle(), TVM_GETNEXTITEM, flag, (LPARAM)(HTREEITEM)item));
}
} // namespace



bool TreeView::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



TreeView::Item::Item(HWND ownerHandle, HTREEITEM item) : _ownerHandle(ownerHandle), _item(item) {
	assert("Invalid treeView handle" && IsWindow(ownerHandle));
}


TreeView::Item& TreeView::Item::operator=(const ItemInfo& itemInfo) {
	assert("Null item handle" && *this);
	if (*this != TVI_ROOT) {// ルートである場合は子項目の追加のみ行う
		ItemData data(*this, itemInfo, itemInfo._itemDataState);
		data.setTo(ownerHandle());
	}
	itemInfos(itemInfo.itemInfos());
	return *this;
}


TreeView::Item TreeView::Item::add(const ItemInfo& itemInfo) {
	assert("Null item handle" && *this);
	Item nextItem(ownerHandle(), TVI_LAST);
	return insert(nextItem, itemInfo);
}


TreeView::Item TreeView::Item::back() {
	assert("Null item handle" && *this);
	Item i = front();
	Item prev(ownerHandle(), nullptr);
	while (i) {
		prev = i;
		i = i.next();
	}
	return prev;
}


const TreeView::Item TreeView::Item::back() const {
	return const_cast<Item*>(this)->back();
}


void TreeView::Item::beginEdit() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	assert("Can't edit treeView" && toFlag(Handle(ownerHandle()).style())[TVS_EDITLABELS]);
	if (ownerHandle() != GetFocus()) {
		SetFocus(ownerHandle());
	}
	verify(SendMessageW(ownerHandle(), TVM_EDITLABELW, 0, (LPARAM)_item));
}


bool TreeView::Item::bold() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return ItemData().getState(*this, TVIS_BOLD) == TVIS_BOLD;
}


void TreeView::Item::bold(bool value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData().setState(*this, TVIS_BOLD, value ? TVIS_BOLD : 0);
}


Rectangle TreeView::Item::bounds() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	RECT rect;
	*reinterpret_cast<HTREEITEM*>(&rect) = *this;
	if (SendMessageW(ownerHandle(), TVM_GETITEMRECT, TRUE, (LPARAM)&rect)) {
		return rect;
	}
	return Rectangle(0, 0, 0, 0);
}


Rectangle TreeView::Item::boundsWithChildren() const {
	assert("Null item handle" && *this);
	Rectangle rect(0, 0, 0, 0);
	if (*this != TVI_ROOT) {
		rect = bounds();
	}
	if (expanded()) {
		auto i = front();
		while (i) {
			rect = Rectangle::or(rect, i.boundsWithChildren());
			i = i.next();
		}
	}
	return rect;
}


bool TreeView::Item::checked() const {
	return state() == 1;
}


void TreeView::Item::checked(bool value) {
	state(value ? 1 : 0);
}


void TreeView::Item::clear() {
	assert("Null item handle" && *this);
	if (*this == TVI_ROOT) {
		DWORD styles = GetWindowLong(ownerHandle(), GWL_STYLE);
		assert(styles);
		verify(SendMessageW(ownerHandle(), TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT));
		if (styles & TVS_NOSCROLL) { // 参考：http://msdn.microsoft.com/en-us/library/bb773790(v=VS.85).aspx
			verify(SetWindowLong(ownerHandle(), GWL_STYLE, styles));
		}
	} else {
		auto i = front();
		while (i) {
			verify(SendMessageW(ownerHandle(), TVM_DELETEITEM, 0, (LPARAM)i._item));
			i = front();
		}
	}
}


bool TreeView::Item::cut() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return ItemData().getState(*this, TVIS_CUT) == TVIS_CUT;
}


void TreeView::Item::cut(bool value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData().setState(*this, TVIS_CUT, value ? TVIS_CUT : 0);
}


TreeView::ItemDescendantsIterator TreeView::Item::descendantsBegin() {
	return ItemDescendantsIterator(*this);
}


void TreeView::Item::endEdit(bool cancel) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	assert("Can't edit treeView" && toFlag(Handle(ownerHandle()).style())[TVS_EDITLABELS]);
	SendMessageW(ownerHandle(), TVM_ENDEDITLABELNOW, cancel ? TRUE : FALSE, 0); // 戻り値はなぜか常にFALSE
}


void TreeView::Item::ensureVisible() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	SendMessageW(ownerHandle(), TVM_ENSUREVISIBLE, 0, (LPARAM)_item);
}


void TreeView::Item::erase(Item& item) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	assert("item is not child" && *this == item.parent());
	verify(SendMessageW(ownerHandle(), TVM_DELETEITEM, 0, (LPARAM)item._item));
	item._item = nullptr;
}


void TreeView::Item::expandAll(bool value) {
	assert("Null item handle" && *this);
	auto i = front();
	while (i) {
		i.expandAll(value);
		i = i.next();
	}
	if (*this != TVI_ROOT) {
		expanded(value);
	}
}


bool TreeView::Item::expanded() const {
	assert("Null item handle" && *this);
	if (*this == TVI_ROOT) {
		return true;
	}
	return ItemData().getState(*this, TVIS_EXPANDED) == TVIS_EXPANDED;
}


void TreeView::Item::expanded(bool value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	SendMessageW(ownerHandle(), TVM_EXPAND, value ? TVE_EXPAND : TVE_COLLAPSE, (WPARAM)_item);
}


TreeView::Item TreeView::Item::front() {
	assert("Null item handle" && *this);
	return Item(ownerHandle(), getNextItem(*this, TVGN_CHILD));
}


const TreeView::Item TreeView::Item::front() const {
	return const_cast<Item*>(this)->front();
}


bool TreeView::Item::highlight() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return ItemData().getState(*this, TVIS_DROPHILITED) == TVIS_DROPHILITED;
}


void TreeView::Item::highlight(bool value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData().setState(*this, TVIS_DROPHILITED, value ? TVIS_DROPHILITED : 0);
}


int TreeView::Item::imageIndex() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData data(*this, TVIF_IMAGE);
	data.getFrom(ownerHandle());
	return data.iImage;
}


void TreeView::Item::imageIndex(int value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData data(*this, TVIF_IMAGE);
	data.iImage = value;
	data.setTo(ownerHandle());
}


TreeView::Item TreeView::Item::insert(Item& nextItem, const ItemInfo& info) {
	assert("Null item handle" && *this);
	assert("nextItem is not child" && (nextItem == TVI_LAST || *this == nextItem.parent()));
	Item item(ownerHandle(), nullptr);
	{// 再帰呼び出しするのでスタック浪費しないように
		TVINSERTSTRUCTW data;
		data.hParent = *this;
		data.hInsertAfter = nextItem;
		data.item = ItemData(nullptr, info, info._itemDataState);
		item._item = reinterpret_cast<HTREEITEM>(SendMessageW(ownerHandle(), TVM_INSERTITEMW, 0, (LPARAM)&data));
		assert("Failed to TVM_INSERTITEMW" && item);
	}
	for (auto i = info.itemInfos().begin(), end = info.itemInfos().end(); i != end; ++i) {
		item.add(*i);
	}
	return item;
}


std::vector<TreeView::ItemInfo> TreeView::Item::itemInfos() const {
	assert("Null item handle" && *this);
	vector<TreeView::ItemInfo> infos;
	auto i = const_cast<Item*>(this)->front();
	while (i) {
		infos.push_back(i);
		i = i.next();
	}
	return infos;
}


void TreeView::Item::itemInfos(ArrayRange<const ItemInfo> value) {
	assert("Null item handle" && *this);
	clear();
	for (auto i = value.begin(), end = value.end(); i != end; ++i) {
		add(*i);
	}
}


TreeView::Item TreeView::Item::next() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return Item(ownerHandle(), getNextItem(*this, TVGN_NEXT));
}


const TreeView::Item TreeView::Item::next() const {
	return const_cast<Item*>(this)->next();
}


TreeView::Item TreeView::Item::nextVisible() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return Item(ownerHandle(), getNextItem(*this, TVGN_NEXTVISIBLE));
}


const TreeView::Item TreeView::Item::nextVisible() const {
	return const_cast<Item*>(this)->nextVisible();
}


HWND TreeView::Item::ownerHandle() const {
	return _ownerHandle;
}


TreeView::Item TreeView::Item::parent() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	auto parent = getNextItem(*this, TVGN_PARENT);
	return Item(ownerHandle(), parent ? parent : TVI_ROOT);
}


const TreeView::Item TreeView::Item::parent() const {
	return const_cast<Item*>(this)->parent();
}


TreeView::Item TreeView::Item::prev() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return Item(ownerHandle(), getNextItem(*this, TVGN_PREVIOUS));
}


const TreeView::Item TreeView::Item::prev() const {
	return const_cast<Item*>(this)->prev();
}


TreeView::Item TreeView::Item::prevVisible() {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	return Item(ownerHandle(), getNextItem(*this, TVGN_PREVIOUSVISIBLE));
}


const TreeView::Item TreeView::Item::prevVisible() const {
	return const_cast<Item*>(this)->prevVisible();
}


int TreeView::Item::selectedImageIndex() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData data(*this, TVIF_SELECTEDIMAGE);
	data.getFrom(ownerHandle());
	return data.iSelectedImage;
}


void TreeView::Item::selectedImageIndex(int value) {
	assert("Null data handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData data(*this, TVIF_SELECTEDIMAGE);
	data.iSelectedImage = value;
	data.setTo(ownerHandle());
}


int TreeView::Item::state() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	int itemDataState = ItemData().getState(*this, TVIS_STATEIMAGEMASK);
	return itemDataStateToState(itemDataState);
}


void TreeView::Item::state(int value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	int itemDataState = stateToItemDataState(value);
	ItemData().setState(*this, TVIS_STATEIMAGEMASK, itemDataState);
}


String TreeView::Item::text() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	wchar_t array[maxTextLength];
	StringBuffer buffer(array);
	textToBuffer(buffer);
	return buffer;
}


void TreeView::Item::text(StringRange value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	assert("Too long text length" && value.length() < maxTextLength);
	ItemData data(*this, TVIF_TEXT);
	data.pszText = const_cast<wchar_t*>(value.c_str());
	data.setTo(ownerHandle());
}


void TreeView::Item::textToBuffer(StringBuffer& buffer) const {
	assert("Null data handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	buffer.reserveAdditionally(maxTextLength - 1);
	ItemData data(*this, TVIF_TEXT);
	data.pszText = buffer.end();
	data.cchTextMax = maxTextLength;
	data.getFrom(ownerHandle());
	buffer.expandLength();
}


void TreeView::Item::userData(UniqueAny&& value) {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData data(*this, TVIF_PARAM);
	data.lParam = 0;
	UniqueAny& any = *reinterpret_cast<UniqueAny*>(&data.lParam);
	any = move(value);
	data.setTo(ownerHandle());
}


bool TreeView::Item::userDataIsEmpty() const {
	return _userData() == nullptr;
}


void* TreeView::Item::_userData() const {
	assert("Null item handle" && *this);
	assert("Can't access TVI_ROOT" && *this != TVI_ROOT);
	ItemData data(*this, TVIF_PARAM);
	data.getFrom(ownerHandle());
	return reinterpret_cast<void*>(data.lParam);
}



TreeView::ItemInfo::ItemInfo(ItemInfo&& value)
	: _text(move(value._text))
	, _itemInfos(move(value._itemInfos))
	, _imageIndex(move(value._imageIndex))
	, _selectedImageIndex(move(value._selectedImageIndex))
	, _itemDataState(move(value._itemDataState))
	{
}


TreeView::ItemInfo::ItemInfo(const Item& item) {
	assert("Null item handle" && item);

	if (item != TVI_ROOT) {
		wchar_t buffer[maxTextLength];
		ItemData data(item, TVIF_TEXT | TVIF_IMAGE | TVIF_STATE | TVIF_SELECTEDIMAGE | TVIF_PARAM);
		data.pszText = buffer;
		data.cchTextMax = maxTextLength;
		data.getFrom(item.ownerHandle());

		_text = buffer;
		_imageIndex = data.iImage;
		_selectedImageIndex = data.iSelectedImage;
		_itemDataState = data.state;
	}
	Item i = const_cast<Item*>(&item)->front();
	while (i) {
		_itemInfos.push_back(i);
		i = i.next();
	}
}


TreeView::ItemInfo::ItemInfo(String text, int imageIndex, int selectedImageIndex, int state)
	: _text(move(text)), _imageIndex(imageIndex), _selectedImageIndex(selectedImageIndex)
	, _itemDataState(stateToItemDataState(state)) {
	assert("Too long text length" && text.length() < maxTextLength);
	assert("imageIndex out of range" && 0 <= imageIndex);
	assert("stateImageIndex out of range" && -1 <= selectedImageIndex);
	assert("state out of range" && -1 <= state);
}


TreeView::ItemInfo::ItemInfo(String text, ArrayRange<const ItemInfo> itemInfos, int imageIndex, int selectedImageIndex, int state)
	: _text(move(text)), _itemInfos(itemInfos.begin(), itemInfos.end()), _imageIndex(imageIndex), _selectedImageIndex(selectedImageIndex)
	, _itemDataState(stateToItemDataState(state)) {
	assert("Too long text length" && text.length() < maxTextLength);
	assert("imageIndex out of range" && 0 <= imageIndex);
	assert("stateImageIndex out of range" && -1 <= selectedImageIndex);
	assert("state out of range" && -1 <= state);
}


TreeView::ItemInfo& TreeView::ItemInfo::operator=(ItemInfo&& value) {
	if (this != &value) {
		_text = move(value._text);
		_itemInfos = move(value._itemInfos);
		_imageIndex = value._imageIndex;
		_selectedImageIndex = value._selectedImageIndex;
		_itemDataState = value._itemDataState;
	}
	return *this;
}


bool TreeView::ItemInfo::bold() const { return toFlag(_itemDataState)[TVIS_BOLD]; }
void TreeView::ItemInfo::bold(bool value) { _itemDataState = toFlag(_itemDataState).set(TVIS_BOLD, value); }
bool TreeView::ItemInfo::cut() const { return toFlag(_itemDataState)[TVIS_CUT]; }
void TreeView::ItemInfo::cut(bool value) { _itemDataState = toFlag(_itemDataState).set(TVIS_CUT, value); }
bool TreeView::ItemInfo::highlight() const { return toFlag(_itemDataState)[TVIS_DROPHILITED]; }
void TreeView::ItemInfo::highlight(bool value) { _itemDataState = toFlag(_itemDataState).set(TVIS_DROPHILITED, value); }
int TreeView::ItemInfo::imageIndex() const { return _imageIndex; }
void TreeView::ItemInfo::imageIndex(int value) {
	assert("imageIndex out of range" && 0 <= value);
	_imageIndex = value;
}
std::vector<TreeView::ItemInfo>& TreeView::ItemInfo::itemInfos() { return _itemInfos; }
const std::vector<TreeView::ItemInfo>& TreeView::ItemInfo::itemInfos() const { return _itemInfos; }
int TreeView::ItemInfo::selectedImageIndex() const { return _selectedImageIndex; }
void TreeView::ItemInfo::selectedImageIndex(int value) {
	assert("selectedImageIndex out of range" && -1 <= value);
	_selectedImageIndex = value;
}
int TreeView::ItemInfo::state() const { return itemDataStateToState(_itemDataState); }
void TreeView::ItemInfo::state(int value) {
	assert("state out of range" && -1 <= value);
	_itemDataState = toFlag(_itemDataState).set(TVIS_STATEIMAGEMASK, false) | stateToItemDataState(value);
}
const String& TreeView::ItemInfo::text() const { return _text; }
void TreeView::ItemInfo::text(String value) {
	assert("Too long text length" && value.length() < maxTextLength);
	_text = move(value);
}



TreeView::ItemDescendantsIterator::ItemDescendantsIterator(Item root) : _root(root), _current(root.front()) {
}


TreeView::ItemDescendantsIterator& TreeView::ItemDescendantsIterator::operator++() {
	assert("Iterator out of range" && *this);
	auto next = _current.front();
	if (next) {
		_current = next;
		return *this;
	}
	next = _current.next();
	if (next) {
		_current = next;
		return *this;
	}
	next = _current;
	for (;;) {
		next = next.parent();
		if (next == _root) {
			_current = Item(_current.ownerHandle(), nullptr);
			break;
		}
		auto parentNext = next.next();
		if (parentNext) {
			_current = parentNext;
			break;
		}
	}
	return *this;
}


TreeView::Item& TreeView::ItemDescendantsIterator::operator*() {
	assert("Iterator out of range" && *this);
	return _current;
}


TreeView::Item* TreeView::ItemDescendantsIterator::operator->() {
	assert("Iterator out of range" && *this);
	return &_current;
}


TreeView::ItemDescendantsIterator::operator bool() const {
	return _current != nullptr;
}



TreeView::ItemEvent::ItemEvent(TreeView& sender, HTREEITEM item)
	: TreeView::Event(sender), _item(item) {
}


TreeView::Item TreeView::ItemEvent::item() { return Item(sender().handle(), _item); }



TreeView::ItemDrag::ItemDrag(TreeView& sender, HTREEITEM item, bool rButton)
	: ItemEvent(sender, item), _rButton(rButton) {
}


bool TreeView::ItemDrag::rButton() const { return _rButton; }



TreeView::ItemExpand::ItemExpand(TreeView& sender, HTREEITEM item, bool expanded)
	: ItemEvent(sender, item), _expanded(expanded) {
}


bool TreeView::ItemExpand::expanded() const { return _expanded; }



TreeView::ItemExpanding::ItemExpanding(TreeView& sender, HTREEITEM item, bool expanded)
	: ItemExpand(sender, item, expanded), _cancel(false) {
}


bool TreeView::ItemExpanding::cancel() const { return _cancel; }
void TreeView::ItemExpanding::cancel(bool value) { _cancel = value; }



TreeView::ItemPrePaint::ItemPrePaint(TreeView& sender, ::tagNMTVCUSTOMDRAW* info)
	: TreeView::Event(sender), _info(info), _fontChanged(false) {
}


Color TreeView::ItemPrePaint::backColor() const { return Color::fromCOLORREF(_info->clrTextBk); }
void TreeView::ItemPrePaint::backColor(const Color& value) { _info->clrTextBk = value.toCOLORREF(); }
Rectangle TreeView::ItemPrePaint::bounds() const { return _info->nmcd.rc; }
bool TreeView::ItemPrePaint::focused() const { return toFlag(_info->nmcd.uItemState)[CDIS_FOCUS]; }
void TreeView::ItemPrePaint::focused(bool value) { _info->nmcd.uItemState = toFlag(_info->nmcd.uItemState).set(CDIS_FOCUS, value); }
Font TreeView::ItemPrePaint::font() const { return Graphics(_info->nmcd.hdc).font(); }
void TreeView::ItemPrePaint::font(HFONT value) { Graphics(_info->nmcd.hdc).font(value); _fontChanged = true; }
TreeView::Item TreeView::ItemPrePaint::item() { return Item(sender(), (HTREEITEM)_info->nmcd.dwItemSpec); }
bool TreeView::ItemPrePaint::selected() const { return toFlag(_info->nmcd.uItemState)[CDIS_SELECTED]; }
void TreeView::ItemPrePaint::selected(bool value) { _info->nmcd.uItemState = toFlag(_info->nmcd.uItemState).set(CDIS_SELECTED, value); }
Color TreeView::ItemPrePaint::textColor() const { return Color::fromCOLORREF(_info->clrText); }
void TreeView::ItemPrePaint::textColor(const Color& value) { _info->clrText = value.toCOLORREF(); }



TreeView::ItemStateChange::ItemStateChange(TreeView& sender, HTREEITEM item, int oldState, int newState)
	: ItemEvent(sender, item), _oldState(oldState), _newState(newState) {
}


int TreeView::ItemStateChange::newState() const { return _newState; }
int TreeView::ItemStateChange::oldState() const { return _oldState; }



TreeView::ItemStateChanging::ItemStateChanging(TreeView& sender, HTREEITEM item, int oldState, int newState)
	: ItemStateChange(sender, item, oldState, newState) {
}


void TreeView::ItemStateChanging::newState(int value) { _newState = value; }



TreeView::ItemTipPopup::ItemTipPopup(TreeView& sender, HTREEITEM item, wchar_t* buffer, int bufferSize)
	: ItemEvent(sender, item), _buffer(buffer), _bufferSize(bufferSize) {
}


void TreeView::ItemTipPopup::setText(StringRange value) {
	int length = min(value.length(), _bufferSize - 1);
	String::refer(value).copyTo(0, _buffer, _bufferSize, 0, length);
	_buffer[length] = L'\0';
}



TreeView::ItemSelect::ItemSelect(TreeView& sender, HTREEITEM oldSelectedItem, HTREEITEM newSelectedItem)
	: TreeView::Event(sender), _oldSelectedItem(oldSelectedItem), _newSelectedItem(newSelectedItem) {
}


TreeView::Item TreeView::ItemSelect::newSelectedItem() const { return Item(sender().handle(), _newSelectedItem); }
TreeView::Item TreeView::ItemSelect::oldSelectedItem() const { return Item(sender().handle(), _oldSelectedItem); }



TreeView::ItemSelecting::ItemSelecting(TreeView& sender, HTREEITEM oldSelectedItem, HTREEITEM newSelectedItem)
	: ItemSelect(sender, oldSelectedItem, newSelectedItem), _cancel(false) {
}


bool TreeView::ItemSelecting::cancel() const { return _cancel; }
void TreeView::ItemSelecting::cancel(bool value) { _cancel = value; }



TreeView::TextEdit::TextEdit(TreeView& sender, HTREEITEM item, String itemText)
	: ItemEvent(sender, item), _itemText(move(itemText)), _cancel(false) {
}


bool TreeView::TextEdit::cancel() const { return _cancel; }
void TreeView::TextEdit::cancel(bool value) { _cancel = value; }
const String& TreeView::TextEdit::itemText() const { return _itemText; }
void TreeView::TextEdit::itemText(String value) { _itemText = move(value); }



TreeView::TextEditing::TextEditing(TreeView& sender, HTREEITEM item, String itemText)
	: TextEdit(sender, item, move(itemText)) {
}


Edit TreeView::TextEditing::edit() {
	Edit edit((HWND)SendMessageW(sender(), TVM_GETEDITCONTROL, 0, 0));
	edit.onShortcutKey() = [&] (Control::ShortcutKey& e) {
		if (e.shortcut() == Key::enter // Enter キーと ESC キーをダイアログキー処理しないようにしておく
		 || e.shortcut() == Key::escape) {
			e.isInputKey(true);
		}
	};
	return edit;
}



TreeView::HitTestInfo::HitTestInfo(HWND handle, HTREEITEM item, int flags)
	: _handle(handle), _item(item), _flags(flags) {
}


TreeView::Item TreeView::HitTestInfo::item() { return Item(_handle, _item); }
bool TreeView::HitTestInfo::onButton() const { return (_flags & TVHT_ONITEMBUTTON) != 0; }
bool TreeView::HitTestInfo::onImage() const { return (_flags & TVHT_ONITEMICON) != 0; }
bool TreeView::HitTestInfo::onIndent() const { return (_flags & TVHT_ONITEMINDENT) != 0; }
bool TreeView::HitTestInfo::onItem() const { return (_flags & TVHT_ONITEM) != 0; }
bool TreeView::HitTestInfo::onItemRight() const { return (_flags & TVHT_ONITEMRIGHT) != 0; }
bool TreeView::HitTestInfo::onStateImage() const { return (_flags & TVHT_ONITEMSTATEICON) != 0; }
bool TreeView::HitTestInfo::onText() const { return (_flags & TVHT_ONITEMLABEL) != 0; }



TreeView::TreeView() {
}


TreeView::TreeView(TreeView&& value, bool checkSlicing)
	: Control(move(value))
	, _onClick(move(value._onClick))
	, _onItemDrag(move(value._onItemDrag))
	, _onItemExpand(move(value._onItemExpand))
	, _onItemExpanding(move(value._onItemExpanding))
	, _onItemPrePaint(move(value._onItemPrePaint))
	, _onItemSelect(move(value._onItemSelect))
	, _onItemSelecting(move(value._onItemSelecting))
	, _onItemStateChange(move(value._onItemStateChange))
	, _onItemStateChanging(move(value._onItemStateChanging))
	, _onItemTipPopup(move(value._onItemTipPopup))
	, _onRightClick(move(value._onRightClick))
	, _onTextEdit(move(value._onTextEdit))
	, _onTextEditing(move(value._onTextEditing))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


TreeView::TreeView(Control& parent, int x, int y, int width, int height, TreeView::Options options) {
	assert("Invalid TreeView::Options" && Options::_validate(options));
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_TREEVIEW_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	if (options & Options::checkBoxes) { // 参考:http://msdn.microsoft.com/en-us/library/bb760013(v=VS.85).aspx
		_handle.style(toFlag(_handle.style()).set(TVS_CHECKBOXES, true));
		_handle.updateStyle();
	}
}


TreeView::TreeView(Control& parent, int x, int y, int width, int height, ArrayRange<const ItemInfo> itemInfos, TreeView::Options options) {
	*this = TreeView(parent, x, y, width, height, options);
	root().itemInfos(itemInfos);
	root().expandAll(true);
	auto newSize = getPreferredSize(width, height);
	root().expandAll(false);
	size(newSize);
}


TreeView::~TreeView() {
	destroyHandle();
}


TreeView& TreeView::operator=(TreeView&& value) {
	if (this != &value) {
		this->~TreeView();
		new (this) TreeView(move(value));
	}
	return *this;
}


bool TreeView::alwaysSelected() const {
	return _handle.hasStyle(TVS_SHOWSELALWAYS);
}


void TreeView::alwaysSelected(bool value) {
	_handle.setStyle(TVS_SHOWSELALWAYS, value);
}


Color TreeView::backColor() const {
	return Color::fromCOLORREF(SendMessageW(handle(), TVM_GETBKCOLOR, 0, 0));
}


void TreeView::backColor(const Color& value) {
	if (value != backColor()) {
		SendMessageW(handle(), TVM_SETBKCOLOR , 0, value.toCOLORREF());
	}
}


bool TreeView::buttons() const {
	return _handle.hasStyle(TVS_HASBUTTONS);
}


void TreeView::buttons(bool value) {
	_handle.setStyle(TVS_HASBUTTONS, value);
}


int TreeView::count() const {
	return SendMessageW(handle(), TVM_GETCOUNT, 0, 0);
}


int TreeView::countPerPage() const {
	return SendMessageW(handle(), TVM_GETVISIBLECOUNT, 0, 0);
}


Control::Edge TreeView::edge() const {
	return _handle.edge();
}


void TreeView::edge(Control::Edge value) {
	_handle.edge(value);
}


bool TreeView::fullRowSelect() const {
	return _handle.hasStyle(TVS_FULLROWSELECT);
}


void TreeView::fullRowSelect(bool value) {
	assert("Can't set fullRowSelect and lines to true" && (!value || !lines()));
	_handle.setStyle(TVS_FULLROWSELECT, value);
}


TreeView::HitTestInfo TreeView::getHitTestInfo(const Point& point) const {
	TVHITTESTINFO info;
	info.pt = point;
	SendMessageW(handle(), TVM_HITTEST, 0, (LPARAM)&info);
	return HitTestInfo(handle(), info.hItem, info.flags);
}


TreeView::HitTestInfo TreeView::getHitTestInfo(int x, int y) const {
	return getHitTestInfo(Point(x, y));
}


TreeView::Item TreeView::getItemAt(const Point& point) {
	auto info = getHitTestInfo(point);
	return info.onItem() ? info.item() : Item(handle(), nullptr);
}


TreeView::Item TreeView::getItemAt(int x, int y) {
	return getItemAt(Point(x, y));
}


Size TreeView::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	auto itemBounds = root().boundsWithChildren();
	auto size = sizeFromClientSize(itemBounds.size());
	return Size(width  ? width  : size.width 
			  , height ? height : size.height);
}


bool TreeView::hotTracking() const {
	return _handle.hasStyle(TVS_TRACKSELECT);
}


void TreeView::hotTracking(bool value) {
	_handle.setStyle(TVS_TRACKSELECT, value);
}


bool TreeView::hScrollable() const {
	return !_handle.hasStyle(TVS_NOHSCROLL);
}


void TreeView::hScrollable(bool value) {
	_handle.setStyle(TVS_NOHSCROLL, !value);
}


ImageList TreeView::imageList() const {
	return ImageList(reinterpret_cast<HIMAGELIST>(SendMessageW(handle(), TVM_GETIMAGELIST, TVSIL_NORMAL, 0)));
}


void TreeView::imageList(HIMAGELIST value) {
	if (value != imageList()) {
		SendMessageW(handle(), TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)value);
	}
}


int TreeView::indent() const {
	return SendMessageW(handle(), TVM_GETINDENT, 0, 0);
}


void TreeView::indent(int value) {
	assert("Negative indent" && 0 <= value);
	if (value != indent()) {
		SendMessageW(handle(), TVM_SETINDENT, value, 0);
	}
}


int TreeView::itemHeight() const {
	return SendMessageW(handle(), TVM_GETITEMHEIGHT, 0, 0);
}


void TreeView::itemHeight(int value) {
	assert("itemHeight out of range" && 0 < value);
	if (value != itemHeight()) {
		auto style = toFlag(_handle.style());
		bool oddable = style[TVS_NONEVENHEIGHT];
		bool odd = (value & 1);
		if (oddable != odd) {
			_handle.style(style.set(TVS_NONEVENHEIGHT, odd));
			_handle.updateStyle();
		}
		SendMessageW(handle(), TVM_SETITEMHEIGHT, value, 0);
	}
}


ToolTip TreeView::itemTip() {
	return ToolTip((HWND)SendMessageW(handle(), TVM_GETTOOLTIPS, 0, 0));
}


bool TreeView::itemTipEnabled() const {
	return _handle.hasStyle(TVS_INFOTIP);
}


void TreeView::itemTipEnabled(bool value) {
	_handle.setStyle(TVS_INFOTIP, value);
}


Color TreeView::lineColor() const {
	return Color::fromCOLORREF(SendMessageW(handle(), TVM_GETLINECOLOR, 0, 0));
}


void TreeView::lineColor(const Color& value) {
	if (value != lineColor()) {
		SendMessageW(handle(), TVM_SETLINECOLOR, 0, value.toCOLORREF());
	}
}


bool TreeView::lines() const {
	return _handle.hasStyle(TVS_HASLINES);
}


void TreeView::lines(bool value) {
	assert("Can't set fullRowSelect and lines to true" && (!value || !fullRowSelect()));
	_handle.setStyle(TVS_HASLINES, value);
}


Listener<TreeView::Click&>& TreeView::onClick() { return _onClick; }
Listener<TreeView::ItemDrag&>& TreeView::onItemDrag() { return _onItemDrag; }
Listener<TreeView::ItemExpand&>& TreeView::onItemExpand() { return _onItemExpand; }
Listener<TreeView::ItemExpanding&>& TreeView::onItemExpanding() { return _onItemExpanding; }
Listener<TreeView::ItemPrePaint&>& TreeView::onItemPrePaint() { return _onItemPrePaint; }
Listener<TreeView::ItemSelect&>& TreeView::onItemSelect() { return _onItemSelect; }
Listener<TreeView::ItemSelecting&>& TreeView::onItemSelecting() { return _onItemSelecting; }
Listener<TreeView::ItemStateChange&>& TreeView::onItemStateChange() { return _onItemStateChange; }
Listener<TreeView::ItemStateChanging&>& TreeView::onItemStateChanging() { return _onItemStateChanging; }
Listener<TreeView::ItemTipPopup&>& TreeView::onItemTipPopup() { return _onItemTipPopup; }
Listener<TreeView::RightClick&>& TreeView::onRightClick() { return _onRightClick; }
Listener<TreeView::TextEdit&>& TreeView::onTextEdit() { return _onTextEdit; }
Listener<TreeView::TextEditing&>& TreeView::onTextEditing() { return _onTextEditing; }


TreeView::Options TreeView::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


TreeView::Item TreeView::root() {
	return Item(handle(), TVI_ROOT);
}


const TreeView::Item TreeView::root() const {
	return const_cast<TreeView*>(this)->root();
}


bool TreeView::rootLines() const {
	return _handle.hasStyle(TVS_LINESATROOT);
}


void TreeView::rootLines(bool value) {
	_handle.setStyle(TVS_LINESATROOT, value);
}


bool TreeView::scrollable() const {
	return !_handle.hasStyle(TVS_NOSCROLL);
}


void TreeView::scrollable(bool value) {
	_handle.setStyle(TVS_NOSCROLL, !value);
}


TreeView::Item TreeView::selectedItem() const {
	auto item = reinterpret_cast<HTREEITEM>(SendMessageW(handle(), TVM_GETNEXTITEM, TVGN_CARET, 0));
	return Item(handle(), item);
}


void TreeView::selectedItem(TreeView::Item value) {
	assert("value is not child item" && value.ownerHandle() == handle());
	assert("Can't select TVI_ROOT" && value != TVI_ROOT);
	verify(SendMessageW(handle(), TVM_SELECTITEM, TVGN_CARET, (LPARAM)(HTREEITEM)value));
}


ImageList TreeView::stateImageList() const {
	return ImageList(reinterpret_cast<HIMAGELIST>(SendMessageW(handle(), TVM_GETIMAGELIST, TVSIL_STATE, 0)));
}


void TreeView::stateImageList(HIMAGELIST value) {
	assert("Can't change stateImageList if checkBoxes is showed" && !(options() & Options::checkBoxes));
	if (value != stateImageList()) {
		SendMessageW(handle(), TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)value);
	}
}


Color TreeView::textColor() const {
	COLORREF color = SendMessageW(handle(), TVM_GETTEXTCOLOR, 0, 0);
	return color == -1 ? Color::controlText() : Color::fromCOLORREF(color);
}


void TreeView::textColor(const Color& value) {
	if (value != textColor()) {
		SendMessageW(handle(), TVM_SETTEXTCOLOR, 0, value.toCOLORREF());
	}
}


bool TreeView::textEditable() const {
	return _handle.hasStyle(TVS_EDITLABELS);
}


void TreeView::textEditable(bool value) {
	_handle.setStyle(TVS_EDITLABELS, value);
}


TreeView::Item TreeView::topItem() const {
	auto item = reinterpret_cast<HTREEITEM>(SendMessageW(handle(), TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0));
	return Item(handle(), item);
}


void TreeView::topItem(TreeView::Item value) {
	assert("value is not child item" && value.ownerHandle() == handle());
	assert("Null item handle" && value);
	assert("Can't access TVI_ROOT" && value != TVI_ROOT);
	verify(SendMessageW(handle(), TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)(HTREEITEM)value));
}


void TreeView::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_DESTROY : {
			if (options() & Options::checkBoxes) {
				verify(ImageList_Destroy(stateImageList()));
			}
			Control::processMessage(msg);
		} break;
		case wmReflect + WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				case NM_CLICK : {
					Click event(*this);
					onClick()(event);
				} break;
				case NM_CUSTOMDRAW : {
					if (onItemPrePaint()) {
						auto info = reinterpret_cast<NMTVCUSTOMDRAW*>(msg.lparam);
						if (info->nmcd.dwDrawStage == CDDS_PREPAINT) {
							msg.result = CDRF_NOTIFYITEMDRAW;
						} else if (info->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
							ItemPrePaint event(*this, info);
							onItemPrePaint()(event);
							if (event._fontChanged) {
								msg.result |= CDRF_NEWFONT;
							}
						}
						return;
					}
				} break;
				case NM_RCLICK : {
					if (onPopupMenu()) {
						SendMessageW(handle(), WM_CONTEXTMENU, (WPARAM)handle(), GetMessagePos());
						msg.result = TRUE;
					}
					RightClick event(*this);
					onRightClick()(event);
				} break;
				case TVN_BEGINDRAG :
				case TVN_BEGINRDRAG : {
					if (onItemDrag()) {
						auto info = (NMTREEVIEWW*)msg.lparam;
						ItemDrag event(*this, info->itemNew.hItem, ((NMHDR*)msg.lparam)->code == TVN_BEGINRDRAG);
						auto item = event.item();
						auto highlight = item.highlight();
						item.highlight(true);
						onItemDrag()(event);
						item.highlight(highlight);
						return;
					}
				} break;
				case TVN_BEGINLABELEDITW : {
					auto info = (NMTVDISPINFOW*)msg.lparam;
					auto editHandle = (HWND)SendMessageW(*this, TVM_GETEDITCONTROL, 0, 0);
					SendMessageW(editHandle, EM_SETLIMITTEXT, maxTextLength - 1, 0);
					TextEditing event(*this, info->item.hItem, String::refer(info->item.pszText));
					onTextEditing()(event);
					msg.result = event.cancel() ? TRUE : FALSE;
					return;
				} break;
				case TVN_DELETEITEMW : {
					auto info = (NMTREEVIEWW*)msg.lparam;
					UniqueAny& any = *reinterpret_cast<UniqueAny*>(&info->itemOld.lParam);
					any.~UniqueAny();
				} break;
				case TVN_ENDLABELEDITW : {
					auto info = (NMTVDISPINFOW*)msg.lparam;
					if (info->item.pszText) {
						TextEdit event(*this, info->item.hItem, String::refer(info->item.pszText));
						onTextEdit()(event);
						if (!event.cancel()) {
							if (event.itemText().c_str() == info->item.pszText) {
								msg.result = TRUE;
							} else {
								Item(handle(), info->item.hItem).text(event.itemText());
							}
						}
					}
					return;
				} break;
				case TVN_GETINFOTIPW : {
					auto info = (NMTVGETINFOTIPW*)msg.lparam;
					ItemTipPopup event(*this, info->hItem, info->pszText, info->cchTextMax);
					onItemTipPopup()(event);
				} break;
				case TVN_ITEMEXPANDINGW : {
					auto info = (NMTREEVIEWW*)msg.lparam;
					ItemExpanding event(*this, info->itemNew.hItem, (info->action & TVE_EXPAND) != 0);
					onItemExpanding()(event);
					msg.result = event.cancel() ? TRUE : FALSE;
					return;
				} break;
				case TVN_ITEMEXPANDEDW : {
					auto info = (NMTREEVIEWW*)msg.lparam;
					ItemExpand event(*this, info->itemNew.hItem, (info->action & TVE_EXPAND) != 0);
					onItemExpand()(event);
				} break;
				case TVN_SELCHANGINGW : {
					auto info = (NMTREEVIEWW*)msg.lparam;
					ItemSelecting event(*this, info->itemOld.hItem, info->itemNew.hItem);
					onItemSelecting()(event);
					msg.result = event.cancel() ? TRUE : FALSE;
					return;
				} break;
				case TVN_SELCHANGEDW : {
					auto info = (NMTREEVIEWW*)msg.lparam;
					ItemSelect event(*this, info->itemOld.hItem, info->itemNew.hItem);
					onItemSelect()(event);
				} break;
			}
			Control::processMessage(msg);
		} break;
		case WM_LBUTTONDBLCLK :
		case WM_LBUTTONDOWN : {
			Point point(GET_X_LPARAM(msg.lparam), GET_Y_LPARAM(msg.lparam));
			auto info = getHitTestInfo(point);
			// チェックボックスをクリックした際に一瞬フォーカスが移ってしまうのを防ぐ。
			if (info.onStateImage()) {
				auto item = info.item();
				int oldState = item.state();
				int newState = oldState + 1;
				// 状態画像リストの最初は何も描画しない状態の画像なので状態の数＋１個の画像がある。
				if (stateImageList().count() - 1 <= newState) {
					newState = 0;
				}
				ItemStateChanging event(*this, item, oldState, newState);
				onItemStateChanging()(event);
				if (event.newState() != event.oldState()) {
					item.state(event.newState());
					onItemStateChange()(event);
				}
				if (msg.message == WM_LBUTTONDOWN) {
					processMouseDown(MouseDown(*this, Mouse::lButton , msg, _dragBox));
				} else {
					MouseDoubleClick doubleClick(*this, Mouse::lButton , msg);
					onMouseDoubleClick()(doubleClick);
				}
			} else {
				Control::processMessage(msg);
			}
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}