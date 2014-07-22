#include "ListView.hpp"

#include <algorithm>
#include <functional>
#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/ImageList.hpp>
#include <balor/gui/Edit.hpp>
#include <balor/gui/ToolTip.hpp>
#include <balor/locale/Locale.hpp>
#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/StringBuffer.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::function;
using std::max;
using std::min;
using std::move;
using std::swap;
using std::vector;
using namespace balor::graphics;
using namespace balor::locale;
using namespace balor::system;


namespace {
static_assert(ListView::Align::left   == LVCFMT_LEFT, "Invalid enum value");
static_assert(ListView::Align::right  == LVCFMT_RIGHT, "Invalid enum value");
static_assert(ListView::Align::center == LVCFMT_CENTER, "Invalid enum value");

static_assert(ListView::ItemPortion::whole        == LVIR_BOUNDS, "Invalid enum value");
static_assert(ListView::ItemPortion::icon         == LVIR_ICON, "Invalid enum value");
static_assert(ListView::ItemPortion::text         == LVIR_LABEL, "Invalid enum value");
static_assert(ListView::ItemPortion::selectBounds == LVIR_SELECTBOUNDS, "Invalid enum value");

static_assert(ListView::Options::checkBoxes    == LVS_EX_CHECKBOXES, "Invalid enum value");
static_assert(ListView::Options::noHeaderClick == LVS_NOSORTHEADER, "Invalid enum value");

static_assert(ListView::Style::largeIcon == LV_VIEW_ICON, "Invalid enum value");
static_assert(ListView::Style::details   == LV_VIEW_DETAILS, "Invalid enum value");
static_assert(ListView::Style::smallIcon == LV_VIEW_SMALLICON, "Invalid enum value");
static_assert(ListView::Style::list      == LV_VIEW_LIST, "Invalid enum value");
static_assert(ListView::Style::tile      == LV_VIEW_TILE, "Invalid enum value");


int getColumnCount(HWND handle) {
	auto header = (HWND)SendMessageW(handle, LVM_GETHEADER, 0, 0);
	auto count = SendMessageW(header, HDM_GETITEMCOUNT, 0, 0);
	assert("Failed to HDM_GETITEMCOUNT" && count != -1);
	return count;
}


int getItemCount(HWND handle) {
	return SendMessageW(handle, LVM_GETITEMCOUNT, 0, 0);
}


int stateToItemDataState(int state) {
	return INDEXTOSTATEIMAGEMASK(state + 1);
}


int itemDataStateToState(int itemDataState) {
	return ((itemDataState & LVIS_STATEIMAGEMASK) >> 12) - 1;
}


struct ColumnData : public LVCOLUMNW {
	ColumnData() {}
	ColumnData(UINT mask) {
		this->mask = mask;
	}
	ColumnData(const ListView::ColumnInfo& info) {
		this->mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		this->fmt = info.align();
		this->pszText = const_cast<wchar_t*>(info.text().c_str());
		this->cx = info.width();
		if (-1 < info.imageIndex()) {
			mask |= LVCF_IMAGE;
			iImage = info.imageIndex();
			fmt |= LVCFMT_IMAGE;
			if (info.rightImage()) {
				fmt |= LVCFMT_BITMAP_ON_RIGHT;
			}
		}
		if (-1 < info.order()) {
			mask |= LVCF_ORDER;
			iOrder = info.order();
		}
	}
	void getFrom(const ListView::Column& column) {
		verify(SendMessageW(column.ownerHandle(), LVM_GETCOLUMNW, column.index(), (LPARAM)this));
	}
	void setTo(ListView::Column& column) {
		verify(SendMessageW(column.ownerHandle(), LVM_SETCOLUMNW, column.index(), (LPARAM)this));
	}
};


struct ItemData : public LVITEMW {
	ItemData() {}
	ItemData(int index, UINT mask) {
		this->iItem = index;
		this->iSubItem = 0;
		this->mask = mask;
	}
	ItemData(int index, const ListView::ItemInfo& info, int state) {
		this->iItem = index;
		this->iSubItem = 0;
		this->mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM;
		this->pszText = const_cast<wchar_t*>(info.texts().empty() ? L"" : info.texts()[0].c_str());
		this->iImage = info.imageIndex();
		this->state = state;
		this->stateMask = LVIS_SELECTED | LVIS_CUT | LVIS_DROPHILITED | LVIS_STATEIMAGEMASK;
		this->iIndent = info.indent();
		this->lParam = 0;
	}

	void getFrom(HWND ownerHandle) {
		verify(SendMessageW(ownerHandle, LVM_GETITEMW, 0, (LPARAM)this));
	}
	void setTo(HWND ownerHandle) {
		verify(SendMessageW(ownerHandle, LVM_SETITEMW, 0, (LPARAM)this));
	}
	int getState(const ListView::Item& item, UINT mask) {
		return SendMessageW(item.ownerHandle(), LVM_GETITEMSTATE, item.index(), mask);
	}
	void setState(ListView::Item& item, UINT mask, UINT value) {
		state = value;
		stateMask = mask;
		verify(SendMessageW(item.ownerHandle(), LVM_SETITEMSTATE, item.index(), (LPARAM)this));
	}
};


bool getExtendedStyle(HWND handle, int flag) {
	return toFlag(SendMessageW(handle, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0))[flag];
}


void setExtendedStyle(HWND handle, int flag, bool value) {
	SendMessageW(handle, LVM_SETEXTENDEDLISTVIEWSTYLE, flag, value ? flag : 0);
}
} // namespace



bool ListView::Align::_validate(Align value) {
	return left <= value && value <= center;
}


bool ListView::ItemPortion::_validate(ItemPortion value) {
	return whole <= value && value <= selectBounds;
}


bool ListView::Style::_validate(Style value) {
	return largeIcon <= value && value <= tile;
}



ListView::Column::Column(HWND ownerHandle, int index) : _ownerHandle(ownerHandle), _index(index) {
	assert("Invalid listView handle" && IsWindow(ownerHandle));
	assert("column index out of range" && 0 <= index);
	assert("column index out of range" && index < getColumnCount(ownerHandle));
}


ListView::Column& ListView::Column::operator=(const ColumnInfo& columnInfo) {
	ColumnData data(columnInfo);
	data.setTo(*this);
	return *this;
}


ListView::Align ListView::Column::align() const {
	ColumnData data(LVCF_FMT);
	data.getFrom(*this);
	return static_cast<Align>(data.fmt & LVCFMT_JUSTIFYMASK);
}


void ListView::Column::align(ListView::Align value) {
	assert("Invalid ListView::Align" && Align::_validate(value));
	ColumnData data(LVCF_FMT);
	data.getFrom(*this);
	data.fmt = (data.fmt & ~LVCFMT_JUSTIFYMASK) | value;
	data.setTo(*this);
	verify(InvalidateRect(ownerHandle(), nullptr, TRUE));
}


void ListView::Column::fitWidthToHeader() {
	width(LVSCW_AUTOSIZE_USEHEADER);
}


void ListView::Column::fitWidthToItem() {
	width(LVSCW_AUTOSIZE);
}


int ListView::Column::imageIndex() const {
	ColumnData data(LVCF_FMT | LVCF_IMAGE);
	data.getFrom(*this);
	return toFlag(data.fmt)[LVCFMT_IMAGE] ? data.iImage : -1;
}


void ListView::Column::imageIndex(int value) {
	assert("imageIndex out of range" && -1 <= value);
	ColumnData data;
	if (value == -1) {
		data.mask = LVCF_FMT;
		data.getFrom(*this);
		data.fmt = toFlag(data.fmt).set(LVCFMT_IMAGE, false);
	} else {
		data.mask = LVCF_IMAGE;
		data.iImage = value;
	}
	data.setTo(*this);
}


int ListView::Column::index() const {
	return _index;
}


int ListView::Column::order() const {
	ColumnData data(LVCF_ORDER);
	data.getFrom(*this);
	return data.iOrder;
}


void ListView::Column::order(int value) {
	assert("order out of range" && 0 <= value);
	assert("order out of range" && value < getColumnCount(ownerHandle()));
	ColumnData data(LVCF_ORDER);
	data.iOrder = value;
	data.setTo(*this);
	verify(InvalidateRect(ownerHandle(), nullptr, TRUE));
}


HWND ListView::Column::ownerHandle() const {
	return _ownerHandle;
}


bool ListView::Column::rightImage() const {
	ColumnData data(LVCF_FMT);
	data.getFrom(*this);
	return toFlag(data.fmt)[LVCFMT_BITMAP_ON_RIGHT];
}


void ListView::Column::rightImage(bool value) {
	ColumnData data(LVCF_FMT);
	data.getFrom(*this);
	data.fmt = toFlag(data.fmt).set(LVCFMT_BITMAP_ON_RIGHT, value);
	data.setTo(*this);
	verify(InvalidateRect(ownerHandle(), nullptr, TRUE));
}


bool ListView::Column::selected() const {
	assert("This function needs visual style" && System::visualStyleEnabled());
	return SendMessageW(ownerHandle(), LVM_GETSELECTEDCOLUMN, 0, 0) == index();
}


void ListView::Column::selected(bool value) {
	assert("This function needs visual style" && System::visualStyleEnabled());
	if (value != selected()) {
		SendMessageW(ownerHandle(), LVM_SETSELECTEDCOLUMN, value ? index() : -1, 0);
	}
}


String ListView::Column::text() const {
	wchar_t buffer[maxTextLength];
	buffer[0] = L'\0';
	ColumnData data(LVCF_TEXT);
	data.pszText = buffer;
	data.cchTextMax = maxTextLength;
	data.getFrom(*this);
	return buffer;
}


void ListView::Column::text(StringRange value) {
	assert("Too long ListView::Column::text length" && value.length() < maxTextLength);
	ColumnData data(LVCF_TEXT);
	data.pszText = const_cast<wchar_t*>(value.c_str());
	data.setTo(*this);
}


int ListView::Column::width() const {
	return SendMessageW(ownerHandle(), LVM_GETCOLUMNWIDTH, index(), 0);
}


void ListView::Column::width(int value) {
	assert("width out of range" && -2 <= value);
	verify(SendMessageW(ownerHandle(), LVM_SETCOLUMNWIDTH, index(), value));
}



ListView::ColumnInfo::ColumnInfo(ColumnInfo&& value)
	: _text(move(value._text))
	, _width(move(value._width))
	, _align(move(value._align))
	, _imageIndex(move(value._imageIndex))
	, _rightImage(move(value._rightImage))
	, _order(move(value._order))
	{
}


ListView::ColumnInfo::ColumnInfo(const Column& column) {
	wchar_t buffer[maxTextLength];
	buffer[0] = L'\0';
	ColumnData data(LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_IMAGE | LVCF_ORDER);
	data.pszText = buffer;
	data.cchTextMax = maxTextLength;
	data.getFrom(column);

	_text = buffer;
	_width = data.cx;
	_align = static_cast<Align>(data.fmt & LVCFMT_JUSTIFYMASK);
	_imageIndex = toFlag(data.fmt)[LVCFMT_IMAGE] ? data.iImage : -1;
	_rightImage = toFlag(data.fmt)[LVCFMT_BITMAP_ON_RIGHT];
	_order = data.iOrder;
}


ListView::ColumnInfo::ColumnInfo(String text, int width, ListView::Align align, int imageIndex, bool rightImage, int order)
	: _text(move(text)), _width(width), _align(align), _imageIndex(imageIndex), _rightImage(rightImage), _order(order) {
	assert("Negative width" && 0 <= width);
	assert("Invalid ListView::Align" && Align::_validate(align));
	assert("imageIndex out of range" && -1 <= imageIndex);
	assert("order out of range" && -1 <= order);
}


ListView::ColumnInfo& ListView::ColumnInfo::operator=(ColumnInfo&& value) {
	if (this != &value) {
		_text = move(value._text);
		_width = value._width;
		_align = value._align;
		_imageIndex = value._imageIndex;
		_rightImage = value._rightImage;
		_order = value._order;
	}
	return *this;
}


ListView::Align ListView::ColumnInfo::align() const { return _align; }
void ListView::ColumnInfo::align(ListView::Align value) {
	assert("Invalid ListView::Align" && Align::_validate(value));
	_align = value;
}
int ListView::ColumnInfo::imageIndex() const { return _imageIndex; }
void ListView::ColumnInfo::imageIndex(int value) {
	assert("imageIndex out of range" && -1 <= value);
	_imageIndex = value;
}
int ListView::ColumnInfo::order() const { return _order; }
void ListView::ColumnInfo::order(int value) {
	assert("order out of range" && -1 <= value);
	_order = value;
}
bool ListView::ColumnInfo::rightImage() const { return _rightImage; }
void ListView::ColumnInfo::rightImage(bool value) { _rightImage = value; }
const String& ListView::ColumnInfo::text() const { return _text; }
void ListView::ColumnInfo::text(String value) { _text = move(value); }
int ListView::ColumnInfo::width() const { return _width; }
void ListView::ColumnInfo::width(int value) {
	assert("Negative width" && 0 <= value);
	_width = value;
}



ListView::Item::Item(HWND ownerHandle, int index) : _ownerHandle(ownerHandle), _index(index) {
	assert("Invalid listView handle" && IsWindow(ownerHandle));
	assert("item index out of range" && 0 <= index);
	assert("item index out of range" && index < getItemCount(ownerHandle));
}


ListView::Item& ListView::Item::operator=(const ItemInfo& itemInfo) {
	ItemData data(index(), itemInfo, itemInfo._itemDataState);
	data.setTo(ownerHandle());
	const int textsSize = itemInfo.texts().size();
	for (int i = 1, end = getColumnCount(ownerHandle()); i < end; ++i) {
		setText(i, i < textsSize ? itemInfo.texts()[i].c_str() : L"");
	}
	return *this;
}


bool ListView::Item::checked() const {
	return state() == 1;
}


void ListView::Item::checked(bool value) {
	state(value ? 1: 0);
}


bool ListView::Item::cut() const {
	return ItemData().getState(*this, LVIS_CUT) == LVIS_CUT;
}


void ListView::Item::cut(bool value) {
	ItemData().setState(*this, LVIS_CUT, value ? LVIS_CUT : 0);
}


void ListView::Item::ensureVisible() {
	verify(SendMessageW(ownerHandle(), LVM_ENSUREVISIBLE, index(), FALSE));
}


Rectangle ListView::Item::getBounds(ListView::ItemPortion portion) const {
	assert("Invalid ListVIew::ItemPortion" && ItemPortion::_validate(portion));
	RECT rect;
	rect.left = portion;
	verify(SendMessageW(ownerHandle(), LVM_GETITEMRECT, index(), (LPARAM)&rect));
	return rect;
}


Rectangle ListView::Item::getSubBounds(int columnIndex) const {
	assert("columnIndex out of range" && 0 <= columnIndex);
	assert("columnIndex out of range" && (!columnIndex || columnIndex < getColumnCount(ownerHandle())));
	RECT rect;
	rect.left = LVIR_BOUNDS;
	rect.top = columnIndex;
	verify(SendMessageW(ownerHandle(), LVM_GETSUBITEMRECT, index(), (LPARAM)&rect));
	return rect;
}


String ListView::Item::getText(int columnIndex) const {
	wchar_t array[maxTextLength];
	StringBuffer buffer(array);
	getTextToBuffer(buffer, columnIndex);
	return buffer;
}


void ListView::Item::getTextToBuffer(StringBuffer& buffer, int columnIndex) const {
	assert("columnIndex out of range" && 0 <= columnIndex);
	assert("columnIndex out of range" && (!columnIndex || columnIndex < getColumnCount(ownerHandle())));
	buffer.reserveAdditionally(maxTextLength - 1);
	ItemData data;
	data.iSubItem = columnIndex;
	data.pszText = buffer.end();
	data.cchTextMax = maxTextLength;
	buffer.expandLength(SendMessageW(ownerHandle(), LVM_GETITEMTEXTW, index(), (LPARAM)&data));
}


bool ListView::Item::highlight() const {
	return ItemData().getState(*this, LVIS_DROPHILITED) == LVIS_DROPHILITED;
}


void ListView::Item::highlight(bool value) {
	ItemData().setState(*this, LVIS_DROPHILITED, value ? LVIS_DROPHILITED : 0);
}


//int ListView::Item::id() const {
//	assert("This function needs visual style" && System::visualStyleEnabled());
//	return ListView_MapIndexToID(ownerHandle(), index());
//}


int ListView::Item::imageIndex() const {
	ItemData data(index(), LVIF_IMAGE);
	data.getFrom(ownerHandle());
	return data.iImage;
}


void ListView::Item::imageIndex(int value) {
	assert("Negative imageIndex" && 0 <= value);
	ItemData data(index(), LVIF_IMAGE);
	data.iImage = value;
	data.setTo(ownerHandle());
}


int ListView::Item::indent() const {
	ItemData data(index(), LVIF_INDENT);
	data.getFrom(ownerHandle());
	return data.iIndent;
}


void ListView::Item::indent(int value) {
	assert("Negative indent" && 0 <= value);
	ItemData data(index(), LVIF_INDENT);
	data.iIndent = value;
	data.setTo(ownerHandle());
}


int ListView::Item::index() const {
	return _index;
}


HWND ListView::Item::ownerHandle() const {
	return _ownerHandle;
}


Point ListView::Item::position() const {
	POINT point;
	verify(SendMessageW(ownerHandle(), LVM_GETITEMPOSITION, index(), (LPARAM)&point));
	return point;
}


void ListView::Item::position(const Point& value) {
	position(value.x, value.y);
}


void ListView::Item::position(int x, int y) {
	POINT point = {x, y};
	SendMessageW(ownerHandle(), LVM_SETITEMPOSITION32, index(), (LPARAM)&point);
}


bool ListView::Item::selected() const {
	return ItemData().getState(*this, LVIS_SELECTED) == LVIS_SELECTED;
}


void ListView::Item::selected(bool value) {
	ItemData().setState(*this, LVIS_SELECTED, value ? LVIS_SELECTED : 0);
}


void ListView::Item::setText(int columnIndex, StringRange text) {
	assert("columnIndex out of range" && 0 <= columnIndex);
	assert("columnIndex out of range" && (!columnIndex || columnIndex < getColumnCount(ownerHandle())));
	assert("Too long text length" && text.length() < maxTextLength);
	ItemData data;
	data.iSubItem = columnIndex;
	data.pszText = const_cast<wchar_t*>(text.c_str());
	verify(SendMessageW(ownerHandle(), LVM_SETITEMTEXTW, index(), (LPARAM)&data));
}


int ListView::Item::state() const {
	int itemDataState = ItemData().getState(*this, LVIS_STATEIMAGEMASK);
	return itemDataStateToState(itemDataState);
}


void ListView::Item::state(int value) {
	int itemDataState = stateToItemDataState(value);
	ItemData().setState(*this, LVIS_STATEIMAGEMASK, itemDataState);
}


String ListView::Item::text() const {
	return getText(0);
}


void ListView::Item::text(StringRange value) {
	setText(0, value);
}


std::vector<String> ListView::Item::texts() const {
	auto count = getColumnCount(ownerHandle());
	vector<String> texts;
	texts.reserve(count);
	for (int i = 0; i < count; ++i) {
		texts.push_back(getText(i));
	}
	return texts;
}


void ListView::Item::texts(StringRangeArray value) {
	auto count = getColumnCount(ownerHandle());
	assert("Too many texts" && value.length() <= count);
	for (int i = 0, end = min(count, value.length()); i < end; ++i) {
		setText(i, value[i]);
	}
	for (int i = value.length(), end = count; i < end; ++i) {
		setText(i, L"");
	}
}


void ListView::Item::userData(UniqueAny&& value) {
	ItemData data(index(), LVIF_PARAM);
	data.lParam = 0;
	UniqueAny& any = *reinterpret_cast<UniqueAny*>(&data.lParam);
	any = move(value);
	data.setTo(ownerHandle());
}


bool ListView::Item::userDataIsEmpty() const {
	return _userData() == nullptr;
}


void* ListView::Item::_userData() const {
	ItemData data(index(), LVIF_PARAM);
	data.getFrom(ownerHandle());
	return reinterpret_cast<void*>(data.lParam);
}



ListView::ItemInfo::ItemInfo(ItemInfo&& value)
	: _texts(move(value._texts))
	, _imageIndex(move(value._imageIndex))
	, _itemDataState(move(value._itemDataState))
	, _indent(move(value._indent)) {
}


ListView::ItemInfo::ItemInfo(const Item& item) {
	wchar_t buffer[maxTextLength];
	buffer[0] = 0;
	ItemData data(item.index(), LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM);
	data.pszText = buffer;
	data.cchTextMax = maxTextLength;
	data.stateMask = LVIS_SELECTED | LVIS_CUT | LVIS_DROPHILITED | LVIS_STATEIMAGEMASK;
	data.getFrom(item.ownerHandle());

	_texts.push_back(buffer);
	_imageIndex = data.iImage;
	_itemDataState = data.state;
	_indent = data.iIndent;

	for (int i = 1, end = getColumnCount(item.ownerHandle()); i < end; ++i) {
		data.iSubItem = i;
		SendMessageW(item.ownerHandle(), LVM_GETITEMTEXTW, item.index(), (LPARAM)&data);
		_texts.push_back(buffer);
	}
}


ListView::ItemInfo::ItemInfo(String text, int imageIndex, int state, int indent)
	: _imageIndex(imageIndex), _itemDataState(stateToItemDataState(state)), _indent(indent) {
	assert("Too long text length" && text.length() < maxTextLength);
	assert("imageIndex out of range" && 0 <= imageIndex);
	assert("state out of range" && -1 <= state);
	assert("indent out of range" && 0 <= indent);
	_texts.push_back(move(text));
}


ListView::ItemInfo::ItemInfo(StringRangeArray texts, int imageIndex, int state, int indent)
	: _imageIndex(imageIndex), _itemDataState(stateToItemDataState(state)), _indent(indent) {
	assert("imageIndex out of range" && 0 <= imageIndex);
	assert("state out of range" && -1 <= state);
	assert("indent out of range" && 0 <= indent);
	this->texts(texts);
}


ListView::ItemInfo& ListView::ItemInfo::operator=(ItemInfo&& value) {
	if (this != &value) {
		_texts = move(value._texts);
		_imageIndex = value._imageIndex;
		_itemDataState = value._itemDataState;
		_indent = value._indent;
	}
	return *this;
}


bool ListView::ItemInfo::cut() const { return toFlag(_itemDataState)[LVIS_CUT]; }
void ListView::ItemInfo::cut(bool value) { _itemDataState = toFlag(_itemDataState).set(LVIS_CUT, value); }
bool ListView::ItemInfo::highlight() const { return toFlag(_itemDataState)[LVIS_DROPHILITED]; }
void ListView::ItemInfo::highlight(bool value) { _itemDataState = toFlag(_itemDataState).set(LVIS_DROPHILITED, value); }
int ListView::ItemInfo::imageIndex() const { return _imageIndex; }
void ListView::ItemInfo::imageIndex(int value) {
	assert("imageIndex out of range" && 0 <= value);
	_imageIndex = value;
}
int ListView::ItemInfo::indent() const { return _indent; }
void ListView::ItemInfo::indent(int value) {
	assert("indent out of range" && 0 <= value);
	_indent = value;
}
int ListView::ItemInfo::state() const { return itemDataStateToState(_itemDataState); }
void ListView::ItemInfo::state(int value) {
	assert("state out of range" && -1 <= value);
	_itemDataState = toFlag(_itemDataState).set(LVIS_STATEIMAGEMASK, false) | stateToItemDataState(value);
}
String ListView::ItemInfo::text() const { return _texts.empty() ? String() : _texts[0]; }
void ListView::ItemInfo::text(String value) {
	assert("Too long text length" && value.length() < maxTextLength);
	if (_texts.empty()) {
		_texts.push_back(move(value));
	} else {
		_texts[0] = move(value);
	}
}
std::vector<String>& ListView::ItemInfo::texts() { return _texts; }
const std::vector<String>& ListView::ItemInfo::texts() const { return _texts; }
void ListView::ItemInfo::texts(StringRangeArray value) {
	_texts.clear();
	_texts.reserve(value.length());
	for (int i = 0, end = value.length(); i < end; ++i) {
		auto text = value[i];
		assert("Too long text length" && text.length() < maxTextLength);
		_texts.push_back(String(text.c_str(), text.length()));
	}
}



ListView::HeaderClick::HeaderClick(ListView& sender, int columnIndex)
	: ListView::Event(sender), _columnIndex(columnIndex) {
}


int ListView::HeaderClick::columnIndex() const { return _columnIndex; }



ListView::ItemEvent::ItemEvent(ListView& sender, int itemIndex)
	: ListView::Event(sender), _itemIndex(itemIndex) {
}


ListView::Item ListView::ItemEvent::item() { return Item(sender(), _itemIndex); }



ListView::ItemClick::ItemClick(ListView& sender, int itemIndex, int columnIndex, const Point& occurredPoint)
	: ItemEvent(sender, itemIndex), _columnIndex(columnIndex), _occurredPoint(occurredPoint) {
}


int ListView::ItemClick::columnIndex() const { return _columnIndex; }
const Point& ListView::ItemClick::occurredPoint() const { return _occurredPoint; }



ListView::ItemChange::ItemChange(ListView& sender, int itemIndex, int oldStates, int newStates)
	: ItemEvent(sender, itemIndex), _oldStates(oldStates), _newStates(newStates) {
}


bool ListView::ItemChange::newFocused() const { return toFlag(_newStates)[LVIS_FOCUSED]; }
bool ListView::ItemChange::newSelected() const { return toFlag(_newStates)[LVIS_SELECTED]; }
int ListView::ItemChange::newState() const { return itemDataStateToState(_newStates); }
bool ListView::ItemChange::oldFocused() const { return toFlag(_oldStates)[LVIS_FOCUSED]; }
bool ListView::ItemChange::oldSelected() const { return toFlag(_oldStates)[LVIS_SELECTED]; }
int ListView::ItemChange::oldState() const { return itemDataStateToState(_oldStates); }



ListView::ItemChanging::ItemChanging(ListView& sender, int itemIndex, int oldStates, int newStates)
	: ItemChange(sender, itemIndex, oldStates, newStates), _cancel(false) {
}


bool ListView::ItemChanging::cancel() const { return _cancel; }
void ListView::ItemChanging::cancel(bool value) { _cancel = value; }



ListView::ItemDrag::ItemDrag(ListView& sender, int itemIndex, bool rButton)
	: ItemEvent(sender, itemIndex), _rButton(rButton) {
}


bool ListView::ItemDrag::rButton() const { return _rButton; }



ListView::ItemPrePaint::ItemPrePaint(ListView& sender, ::tagNMLVCUSTOMDRAW* info)
	: ListView::Event(sender), _info(info), _fontChanged(false) {
}


Color ListView::ItemPrePaint::backColor() const { return Color::fromCOLORREF(_info->clrTextBk); }
void ListView::ItemPrePaint::backColor(const Color& value) { _info->clrTextBk = value.toCOLORREF(); }
Rectangle ListView::ItemPrePaint::bounds() const { return _info->nmcd.rc; }
int ListView::ItemPrePaint::columnIndex() const { return _info->iSubItem; }
bool ListView::ItemPrePaint::focused() const { return toFlag(_info->nmcd.uItemState)[CDIS_FOCUS]; }
void ListView::ItemPrePaint::focused(bool value) { _info->nmcd.uItemState = toFlag(_info->nmcd.uItemState).set(CDIS_FOCUS, value); }
Font ListView::ItemPrePaint::font() const { return Graphics(_info->nmcd.hdc).font(); }
void ListView::ItemPrePaint::font(HFONT value) { Graphics(_info->nmcd.hdc).font(value); _fontChanged = true; }
ListView::Item ListView::ItemPrePaint::item() { return Item(sender(), _info->nmcd.dwItemSpec); }
bool ListView::ItemPrePaint::selected() const { return toFlag(_info->nmcd.uItemState)[CDIS_SELECTED]; }
void ListView::ItemPrePaint::selected(bool value) { _info->nmcd.uItemState = toFlag(_info->nmcd.uItemState).set(CDIS_SELECTED, value); }
Color ListView::ItemPrePaint::textColor() const { return Color::fromCOLORREF(_info->clrText); }
void ListView::ItemPrePaint::textColor(const Color& value) { _info->clrText = value.toCOLORREF(); }



ListView::ItemTipPopup::ItemTipPopup(ListView& sender, int itemIndex, wchar_t* buffer, int bufferSize)
	: ItemEvent(sender, itemIndex), _buffer(buffer), _bufferSize(bufferSize) {
}


void ListView::ItemTipPopup::setText(StringRange value) {
	int length = min(value.length(), _bufferSize - 1);
	String::refer(value).copyTo(0, _buffer, _bufferSize, 0, length);
	_buffer[length] = L'\0';
}



ListView::TextEdit::TextEdit(ListView& sender, int itemIndex, String itemText)
	: ItemEvent(sender, itemIndex), _itemText(move(itemText)), _cancel(false) {
}


bool ListView::TextEdit::cancel() const { return _cancel; }
void ListView::TextEdit::cancel(bool value) { _cancel = value; }
const String& ListView::TextEdit::itemText() const { return _itemText; }
void ListView::TextEdit::itemText(String value) { _itemText = move(value); }



ListView::TextEditing::TextEditing(ListView& sender, int itemIndex, String itemText)
	: TextEdit(sender, itemIndex, move(itemText)) {
}


Edit ListView::TextEditing::edit() {
	Edit edit((HWND)SendMessageW(sender(), LVM_GETEDITCONTROL, 0, 0));
	edit.onShortcutKey() = [&] (Control::ShortcutKey& e) {
		if (e.shortcut() == Key::enter // Enter キ?と ESC キ?を?イアログキ?処理しないようにしておく
		 || e.shortcut() == Key::escape) {
			e.isInputKey(true);
		}
	};
	return edit;
}



ListView::HitTestInfo::HitTestInfo(int itemIndex, int columnIndex, int flags)
	: _itemIndex(itemIndex), _columnIndex(columnIndex), _flags(flags) {
}


int ListView::HitTestInfo::columnIndex() const { return _columnIndex; }
int ListView::HitTestInfo::itemIndex() const  { return _itemIndex; }
bool ListView::HitTestInfo::onImage() const { return toFlag(_flags)[LVHT_ONITEMICON]; }
bool ListView::HitTestInfo::onStateImage() const { return toFlag(_flags)[LVHT_ONITEMSTATEICON]; }
bool ListView::HitTestInfo::onText() const { return toFlag(_flags)[LVHT_ONITEMLABEL]; }



ListView::ListView() {
}


ListView::ListView(ListView&& value, bool checkSlicing)
	: Control(move(value))
	, _onHeaderClick(move(value._onHeaderClick))
	, _onItemChange(move(value._onItemChange))
	, _onItemChanging(move(value._onItemChanging))
	, _onItemClick(move(value._onItemClick))
	, _onItemDoubleClick(move(value._onItemDoubleClick))
	, _onItemDrag(move(value._onItemDrag))
	, _onItemPrePaint(move(value._onItemPrePaint))
	, _onItemRightClick(move(value._onItemRightClick))
	, _onItemTipPopup(move(value._onItemTipPopup))
	, _onTextEdit(move(value._onTextEdit))
	, _onTextEditing(move(value._onTextEditing))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ListView::ListView(Control& parent, int x, int y, int width, int height, ListView::Style style, ListView::Options options) {
	assert("Invalid ListView::Style" && Style::_validate(style));
	assert("ListView::Style::tile needs visual style" && (style != Style::tile) || System::visualStyleEnabled());

	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_LISTVIEW_CLASSES};
	verify(InitCommonControlsEx(&init));
	bool tile = style == Style::tile;
	if (tile) {
		style = Style::largeIcon; // ?イルはウインドウス?イルではない
	}
	attachHandle(CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL
		| LVS_SHAREIMAGELISTS | LVS_AUTOARRANGE | style | (options & Options::noHeaderClick)
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	if (tile) {
		this->style(Style::tile);
	}
	// ?ェック?ックスの?示は変更できるが?目があると?示が乱れたり、stateImageList との共存など問題が多いので変更不可にする。
	if (options & Options::checkBoxes) {
		setExtendedStyle(handle(), LVS_EX_CHECKBOXES, true);
	}
}


ListView::ListView(Control& parent, int x, int y, int width, int height, ArrayRange<const ItemInfo> itemInfos, ListView::Style style, ListView::Options options) {
	*this = ListView(parent, x, y, width, height, style, options);
	this->itemInfos(itemInfos);
}


ListView::ListView(Control& parent, int x, int y, int width, int height, ArrayRange<const ColumnInfo> columnInfos, ArrayRange<const ItemInfo> itemInfos, ListView::Style style, ListView::Options options) {
	*this = ListView(parent, x, y, width, height, style, options);
	this->columnInfos(columnInfos);
	this->itemInfos(itemInfos);
	size(getPreferredSize(width, height));
}


ListView::~ListView() {
	destroyHandle();
}


ListView& ListView::operator=(ListView&& value) {
	if (this != &value) {
		this->~ListView();
		new (this) ListView(move(value));
	}
	return *this;
}


void ListView::add(const ItemInfo& itemInfo) {
	insert(count(), itemInfo);
}


bool ListView::alwaysSelected() const {
	return _handle.hasStyle(LVS_SHOWSELALWAYS);
}


void ListView::alwaysSelected(bool value) {
	_handle.setStyle(LVS_SHOWSELALWAYS, value);
}


void ListView::arrange(bool snapToGrid) {
	auto style = this->style();
	if (style != Style::details && style != Style::list) {
		verify(SendMessageW(handle(), LVM_ARRANGE, snapToGrid ? LVA_SNAPTOGRID : LVA_DEFAULT, 0));
	}
}


bool ListView::autoArrange() const {
	return _handle.hasStyle(LVS_AUTOARRANGE);
}


void ListView::autoArrange(bool value) {
	_handle.setStyle(LVS_AUTOARRANGE, value);
}


void ListView::beginEdit(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	assert("Can't edit listView" && textEditable());
	if (!focused()) {
		focus();
	}
	verify(SendMessageW(handle(), LVM_EDITLABELW, index, 0));
}


void ListView::brush(HBRUSH value) {
	Control::brush(value);
	LVBKIMAGEW image;
	ZeroMemory(&image, sizeof(image));
	if (value) {
		Brush brush(value);
		Bitmap bitmap = brush.bitmap();
		if (bitmap) {
			bitmap = bitmap.clone();
			image.hbm = bitmap;
			image.ulFlags = LVBKIF_SOURCE_HBITMAP | LVBKIF_STYLE_TILE;
			auto origin = brushOrigin();
			if (origin != Point(0, 0)) {
				image.ulFlags |= LVBKIF_FLAG_TILEOFFSET;
				image.xOffsetPercent = -origin.x;
				image.yOffsetPercent = -origin.y;
			}
			verify(SendMessageW(handle(), LVM_SETBKIMAGE, 0, (LPARAM)&image));
			bitmap.owned(false); // 所有権はリストビュ?へ
			verify(SendMessageW(handle(), LVM_SETTEXTBKCOLOR, 0, CLR_NONE));
		} else {
			verify(!SendMessageW(handle(), LVM_SETBKIMAGE, 0, (LPARAM)&image));
			verify(SendMessageW(handle(), LVM_SETBKCOLOR, 0, brush.color().toCOLORREF()));
			verify(SendMessageW(handle(), LVM_SETTEXTBKCOLOR, 0, CLR_NONE));
		}
	} else {
		LVBKIMAGEW image;
		ZeroMemory(&image, sizeof(image));
		verify(!SendMessageW(handle(), LVM_SETBKIMAGE, 0, (LPARAM)&image));
		verify(SendMessageW(handle(), LVM_SETBKCOLOR, 0, Color::window().toCOLORREF()));
	}
}


void ListView::brushOrigin(const Point& value) {
	Control::brushOrigin(value);
	auto brush = this->brush();
	if (brush) {
		this->brush(brush);
	}
}


void ListView::clear() {
	verify(SendMessageW(handle(), LVM_DELETEALLITEMS, 0, 0));
}


int ListView::columnCount() const {
	return getColumnCount(handle());
}


vector<ListView::ColumnInfo> ListView::columnInfos() const {
	vector<ColumnInfo> infos;
	int count = columnCount();
	infos.reserve(count);
	for (int i = 0; i < count; ++i) {
		infos.push_back(Column(handle(), i));
	}
	return infos;
}


void ListView::columnInfos(ArrayRange<const ListView::ColumnInfo> value) {
	int count = columnCount();
	for (int i = 0, end = value.size(); i < end; ++i) {
		const ColumnInfo& info = value[i];
		ColumnData data(info);
		if (i < count) {
			verify(SendMessageW(handle(), LVM_SETCOLUMNW, i, (LPARAM)&data));
		} else {
			verify(SendMessageW(handle(), LVM_INSERTCOLUMNW, i, (LPARAM)&data) != -1);
		}
	}
	for (int i = value.size(), end = count; i < end; ++i) {
		verify(SendMessageW(handle(), LVM_DELETECOLUMN, i, 0));
	}
	for (int i = 0, end = value.size(); i < end; ++i) {
		if (!value[i].width()) {
			getColumn(i).fitWidthToHeader();
		}
	}
}


int ListView::count() const {
	return SendMessageW(handle(), LVM_GETITEMCOUNT, 0, 0);
}


int ListView::countPerPage() const {
	assert("Invalid style" && (style() == Style::details || style() == Style::list));
	return SendMessageW(handle(), LVM_GETCOUNTPERPAGE, 0, 0);
}


Control::Edge ListView::edge() const {
	return _handle.edge();
}


void ListView::edge(Control::Edge value) {
	_handle.edge(value);
}


void ListView::endEdit() {
	SendMessageW(handle(), LVM_EDITLABELW, (WPARAM)-1, 0);
}


void ListView::erase(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	verify(SendMessageW(handle(), LVM_DELETEITEM, index, 0));
}


int ListView::findIndex(StringRange text, int startIndex, bool prefixSearch) const {
	//assert("Empty text" && !text.empty());
	assert("startIndex out of range" && 0 <= startIndex);
	assert("startIndex out of range" && startIndex <= count());
	LVFINDINFOW info;
	info.flags = prefixSearch ? (LVFI_STRING | LVFI_PARTIAL) : LVFI_STRING;
	info.psz = text.c_str();
	return SendMessageW(handle(), LVM_FINDITEMW, startIndex - 1, (LPARAM)&info);
}


int ListView::focusedIndex() const {
	return SendMessageW(handle(), LVM_GETNEXTITEM, (WPARAM)-1, LVNI_FOCUSED);
}


void ListView::focusedIndex(int value) {
	assert("index out of range" && -1 <= value);
	assert("index out of range" && value < count());
	if (value != focusedIndex()) {
		ItemData data;
		data.stateMask = LVIS_FOCUSED;
		if (value == -1) {
			data.state = 0;
			value = focusedIndex();
		} else {
			data.state = LVIS_FOCUSED;
		}
		verify(SendMessageW(handle(), LVM_SETITEMSTATE, value, (LPARAM)&data));
	}
}


bool ListView::fullRowSelect() const {
	return getExtendedStyle(handle(), LVS_EX_FULLROWSELECT);
}


void ListView::fullRowSelect(bool value) {
	setExtendedStyle(handle(), LVS_EX_FULLROWSELECT, value);
}


ListView::Column ListView::getColumn(int index) {
	return Column(handle(), index);
}


ListView::HitTestInfo ListView::getHitTestInfo(const Point& point) const {
	LVHITTESTINFO info;
	info.pt = point;
	info.iSubItem = 0;
	info.flags = 0;
	if (style() == Style::details) {
		SendMessageW(handle(), LVM_SUBITEMHITTEST, 0, (LPARAM)&info);
	} else {
		SendMessageW(handle(), LVM_HITTEST, 0, (LPARAM)&info);
	}
	// LVHT_ONITEMSTATEICON と LVHT_ABOVE は同じ値なのでかぶらないようにはじく
	return HitTestInfo(info.iItem, info.iSubItem, info.iItem == -1 ? 0 : info.flags);
}


ListView::HitTestInfo ListView::getHitTestInfo(int x, int y) const {
	return getHitTestInfo(Point(x, y));
}


int ListView::getIndexAt(const Point& point) const {
	LVHITTESTINFO info;
	info.pt = point;
	int index = SendMessageW(handle(), LVM_HITTEST, 0, (LPARAM)&info);
	return index != -1 && info.flags & LVHT_ONITEM ? index : -1;

}


int ListView::getIndexAt(int x, int y) const {
	return getIndexAt(Point(x, y));
}


//int ListView::getIndexFromItemId(int id) {
//	assert("This function needs visual style" && System::visualStyleEnabled());
//	return ListView_MapIDToIndex(handle(), id);
//}


Size ListView::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	auto result = SendMessageW(handle(), LVM_APPROXIMATEVIEWRECT, (WPARAM)-1, MAKELPARAM(-1, -1));
	auto size = sizeFromClientSize(Size(LOWORD(result), HIWORD(result)));
	return Size(width  ? width  : size.width 
			   ,height ? height : size.height);
}


int ListView::getSelectedIndex(int startIndex) const {
	assert("startIndex out of range" && 0 <= startIndex);
	assert("startIndex out of range" && startIndex <= count());
	return SendMessageW(handle(), LVM_GETNEXTITEM, startIndex - 1, LVNI_SELECTED);
}


bool ListView::gridLines() const {
	return getExtendedStyle(handle(), LVS_EX_GRIDLINES);
}


void ListView::gridLines(bool value) {
	setExtendedStyle(handle(), LVS_EX_GRIDLINES, value);
}


bool ListView::headerDragDrop() const {
	return getExtendedStyle(handle(), LVS_EX_HEADERDRAGDROP);
}


void ListView::headerDragDrop(bool value) {
	setExtendedStyle(handle(), LVS_EX_HEADERDRAGDROP, value);
}


bool ListView::headerVisible() const {
	return !_handle.hasStyle(LVS_NOCOLUMNHEADER);
}


void ListView::headerVisible(bool value) {
	_handle.setStyle(LVS_NOCOLUMNHEADER, !value);
}


bool ListView::hotTracking() const {
	return getExtendedStyle(handle(), LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT);
}


void ListView::hotTracking(bool value) {
	setExtendedStyle(handle(), LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT, value);
}


bool ListView::hotTrackSelection() const {
	return getExtendedStyle(handle(), LVS_EX_TRACKSELECT);
}


void ListView::hotTrackSelection(bool value) {
	setExtendedStyle(handle(), LVS_EX_TRACKSELECT, value);
}


Size ListView::iconSpacing() const {
	assert("Invalid style" && style() == Style::largeIcon);
	auto result = SendMessageW(handle(), LVM_GETITEMSPACING, FALSE, 0);
	return Size(LOWORD(result), HIWORD(result));
}


void ListView::iconSpacing(const Size& value) {
	assert("Invalid style" && style() == Style::largeIcon);
	SendMessageW(handle(), LVM_SETICONSPACING, value.width, value.height);
	invalidate();
}


void ListView::insert(int index, const ItemInfo& itemInfo) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= count());
	ItemData data(index, itemInfo, itemInfo._itemDataState);
	verify(SendMessageW(handle(), LVM_INSERTITEMW, 0, (LPARAM)&data) != -1);
	data.mask = LVIF_TEXT;
	const int textsSize = itemInfo.texts().size();
	for (int i = 1, end = columnCount(); i < end; ++i) {
		data.iSubItem = i;
		data.pszText = const_cast<wchar_t*>(i < textsSize ? itemInfo.texts()[i].c_str() : L"");
		verify(SendMessageW(handle(), LVM_SETITEMTEXTW, index, (LPARAM)&data));
	}
}


vector<ListView::ItemInfo> ListView::itemInfos() const {
	vector<ItemInfo> infos;
	int count = this->count();
	infos.reserve(count);
	for (int i = 0; i < count; ++i) {
		infos.push_back(Item(handle(), i));
	}
	return infos;
}


void ListView::itemInfos(ArrayRange<const ListView::ItemInfo> value) {
	const int count = this->count();
	const int valueSize = value.size();
	for (int i = 0, end = min(valueSize, count); i < end; ++i) {
		(*this)[i] = value[i];
	}
	for (int i = count, end = valueSize; i < end; ++i) {
		insert(i, value[i]);
	}
	for (int i = valueSize, end = count; i < end; ++i) {
		erase(i);
	}
}


ToolTip ListView::itemTip() {
	return ToolTip((HWND)SendMessageW(handle(), LVM_GETTOOLTIPS, 0, 0));
}


bool ListView::itemTipEnabled() const {
	return getExtendedStyle(handle(), LVS_EX_INFOTIP);
}


void ListView::itemTipEnabled(bool value) {
	setExtendedStyle(handle(), LVS_EX_INFOTIP, value);
}


ImageList ListView::largeImageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), LVM_GETIMAGELIST, LVSIL_NORMAL, 0));
}


void ListView::largeImageList(HIMAGELIST value) {
	if (value != largeImageList()) {
		SendMessageW(handle(), LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)value);
	}
}


bool ListView::leftArrange() const {
	return _handle.hasStyle(LVS_ALIGNLEFT);
}


void ListView::leftArrange(bool value) {
	_handle.setStyle(LVS_ALIGNLEFT, value);
	arrange();
}


bool ListView::multiselect() const {
	return !_handle.hasStyle(LVS_SINGLESEL);
}


void ListView::multiselect(bool value) {
	_handle.setStyle(LVS_SINGLESEL, !value);
}


Listener<ListView::HeaderClick&>& ListView::onHeaderClick() { return _onHeaderClick; }
Listener<ListView::ItemChange&>& ListView::onItemChange() { return _onItemChange; }
Listener<ListView::ItemChanging&>& ListView::onItemChanging() { return _onItemChanging; }
Listener<ListView::ItemClick&>& ListView::onItemClick() { return _onItemClick; }
Listener<ListView::ItemDoubleClick&>& ListView::onItemDoubleClick() { return _onItemDoubleClick; }
Listener<ListView::ItemDrag&>& ListView::onItemDrag() { return _onItemDrag; }
Listener<ListView::ItemPrePaint&>& ListView::onItemPrePaint() { return _onItemPrePaint; }
Listener<ListView::ItemRightClick&>& ListView::onItemRightClick() { return _onItemRightClick; }
Listener<ListView::ItemTipPopup&>& ListView::onItemTipPopup() { return _onItemTipPopup; }
Listener<ListView::TextEdit&>& ListView::onTextEdit() { return _onTextEdit; }
Listener<ListView::TextEditing&>& ListView::onTextEditing() { return _onTextEditing; }


ListView::Options ListView::options() const {
	auto options =  static_cast<Options>(_handle.style() & Options::noHeaderClick);
	if (getExtendedStyle(handle(), LVS_EX_CHECKBOXES)) {
		options |= Options::checkBoxes;
	}
	return options;
}


bool ListView::scrollable() const {
	return !_handle.hasStyle(LVS_NOSCROLL);
}


void ListView::scrollable(bool value) {
	_handle.setStyle(LVS_NOSCROLL, !value);
}


int ListView::selectedCount() const {
	return SendMessageW(handle(), LVM_GETSELECTEDCOUNT, 0, 0);
}


ImageList ListView::smallImageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), LVM_GETIMAGELIST, LVSIL_SMALL, 0));
}


void ListView::smallImageList(HIMAGELIST value) {
	if (value != smallImageList()) {
		SendMessageW(handle(), LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)value);
	}
}


void ListView::sort(bool ascending, int columnIndex) {
	if (ascending) {
		sort([] (const String& lhs, const String& rhs) { return String::compare(lhs, rhs, Locale::current()); }, columnIndex);
	} else {
		sort([] (const String& lhs, const String& rhs) { return String::compare(rhs, lhs, Locale::current()); }, columnIndex);
	}
}


void ListView::sort(const std::function<int (int, int)>& compareFunction) {
	struct Function {
		static int CALLBACK compare(LPARAM lhs, LPARAM rhs, LPARAM functionPointer) {
			return (*(function<int (int, int)>*)functionPointer)(lhs, rhs);
		}
	};
	verify(SendMessageW(handle(), LVM_SORTITEMSEX, (WPARAM)&compareFunction, (LPARAM)Function::compare));
}


void ListView::sort(const std::function<int (const String&, const String&)>& compareFunction, int columnIndex) {
	sort([&] (int lIndex, int rIndex) -> int {
		wchar_t larray[maxTextLength];
		wchar_t rarray[maxTextLength];
		StringBuffer lbuffer(larray);
		StringBuffer rbuffer(rarray);
		(*this)[lIndex].getTextToBuffer(lbuffer, columnIndex);
		(*this)[rIndex].getTextToBuffer(rbuffer, columnIndex);
		return compareFunction(String::refer(lbuffer), String::refer(rbuffer));
	});
}


ImageList ListView::stateImageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), LVM_GETIMAGELIST, LVSIL_STATE, 0));
}


void ListView::stateImageList(HIMAGELIST value) {
	assert("Can't change stateImageList if checkBoxes are showed" && !(options() & Options::checkBoxes));
	if (value != stateImageList()) {
		SendMessageW(handle(), LVM_SETIMAGELIST, LVSIL_STATE, (LPARAM)value);
	}
}


ListView::Style ListView::style() const {
	if (System::visualStyleEnabled()) {
		return static_cast<Style>(SendMessageW(handle(), LVM_GETVIEW, 0, 0));
	} else {
		return static_cast<Style>(_handle.style() & LVS_TYPEMASK);
	}
}


void ListView::style(ListView::Style value) {
	assert("This function needs visual style" && System::visualStyleEnabled());
	assert("Invalid ListView::Style" && Style::_validate(value));
	if (value != style()) {
		verify(SendMessageW(handle(), LVM_SETVIEW, value, 0) == 1);
	}
}


Color ListView::textColor() const {
	return Color::fromCOLORREF(SendMessageW(handle(), LVM_GETTEXTCOLOR, 0, 0));
}


void ListView::textColor(const Color& value) {
	if (value != textColor()) {
		verify(SendMessageW(handle(), LVM_SETTEXTCOLOR, 0, value.toCOLORREF()));
		invalidate();
	}
}


bool ListView::textEditable() const {
	return _handle.hasStyle(LVS_EDITLABELS);
}


void ListView::textEditable(bool value) {
	_handle.setStyle(LVS_EDITLABELS, value);
}


bool ListView::textVisible() const {
	return !getExtendedStyle(handle(), LVS_EX_HIDELABELS);
}


void ListView::textVisible(bool value) {
	assert("This function needs visual style" && System::visualStyleEnabled());
	setExtendedStyle(handle(), LVS_EX_HIDELABELS, !value);
}


bool ListView::textWrap() const {
	return !_handle.hasStyle(LVS_NOLABELWRAP);
}


void ListView::textWrap(bool value) {
	_handle.setStyle(LVS_NOLABELWRAP, !value);
}


int ListView::topIndex() const {
	assert("Can't get topIndex" && (style() == Style::details || style() == Style::list));
	return SendMessageW(handle(), LVM_GETTOPINDEX, 0, 0);
}


void ListView::topIndex(int value) {
	assert("Can't set topIndex" && (style() == Style::details || style() == Style::list));
	auto oldTopIndex = topIndex();
	auto oldPos = (*this)[oldTopIndex].position();
	auto newPos = (*this)[value].position();

	Point diff;
	if (style() == Style::list) {
		diff.x = newPos.x - oldPos.x;
		diff.y = 0;
	} else {
		diff.x = newPos.x - 8;
		auto stateImage = stateImageList();
		if (stateImage) {
			diff.x -= stateImage.imageSize().width;
		}
		diff.y = newPos.y - oldPos.y;
	}
	verify(SendMessageW(handle(), LVM_SCROLL, diff.x, diff.y));
}


ListView::Item ListView::operator[](int index) {
	return Item(handle(), index);
}


void ListView::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				case LVN_BEGINDRAG : {
					auto info = (NMLISTVIEW*)msg.lparam;
					ItemDrag event(*this, info->iItem, false);
					onItemDrag()(event);
				} break;
				case LVN_BEGINRDRAG : {
					auto info = (NMLISTVIEW*)msg.lparam;
					ItemDrag event(*this, info->iItem, true);
					onItemDrag()(event);
				} break;
				case LVN_BEGINLABELEDITW : {
					auto info = (LV_DISPINFOW*)msg.lparam;
					auto editHandle = (HWND)SendMessageW(*this, LVM_GETEDITCONTROL, 0, 0);
					SendMessageW(editHandle, EM_SETLIMITTEXT, maxTextLength - 1, 0);
					TextEditing event(*this, info->item.iItem, String());
					onTextEditing()(event);
					msg.result = event.cancel() ? TRUE : FALSE;
					return;
				} break;
				case LVN_COLUMNCLICK : {
					auto info = (NMLISTVIEW*)msg.lparam;
					HeaderClick event(*this, info->iSubItem);
					onHeaderClick()(event);
				} break;
				case LVN_DELETEITEM : {
					auto info = (NMLISTVIEW*)msg.lparam;
					UniqueAny& any = *reinterpret_cast<UniqueAny*>(&info->lParam);
					any.~UniqueAny();
				} break;
				case LVN_ENDLABELEDITW : {
					auto info = (LV_DISPINFOW*)msg.lparam;
					if (info->item.pszText) {
						TextEdit event(*this, info->item.iItem, String::refer(info->item.pszText));
						onTextEdit()(event);
						if (!event.cancel()) {
							if (event.itemText().c_str() == info->item.pszText) {
								msg.result = TRUE;
							} else {
								(*this)[info->item.iItem].setText(0, event.itemText());
							}
						}
					}
					return;
				} break;
				case LVN_GETINFOTIPW : {
					auto info = (NMLVGETINFOTIPW*)msg.lparam;
					ItemTipPopup event(*this, info->iItem, info->pszText, info->cchTextMax);
					onItemTipPopup()(event);
				} break;
				case LVN_ITEMCHANGED : {
					auto info = (NMLISTVIEW*)msg.lparam;
					if (info->uChanged & LVIF_STATE) {
						int oldState = info->uOldState & (LVIS_STATEIMAGEMASK | LVIS_SELECTED | LVIS_FOCUSED);
						int newState = info->uNewState & (LVIS_STATEIMAGEMASK | LVIS_SELECTED | LVIS_FOCUSED);
						if (oldState != newState) {
							ItemChange event(*this, info->iItem, oldState, newState);
							onItemChange()(event);
						}
					}
				} break;
				case LVN_ITEMCHANGING : {
					auto info = (NMLISTVIEW*)msg.lparam;
					if (info->uChanged & LVIF_STATE) {
						int oldState = info->uOldState & (LVIS_STATEIMAGEMASK | LVIS_SELECTED | LVIS_FOCUSED);
						int newState = info->uNewState & (LVIS_STATEIMAGEMASK | LVIS_SELECTED | LVIS_FOCUSED);
						if (oldState != newState) {
							ItemChanging event(*this, info->iItem, oldState, newState);
							onItemChanging()(event);
							msg.result = event.cancel() ? TRUE : FALSE;
						}
					}
					return;
				} break;
				case NM_CLICK : {
					auto info = (NMITEMACTIVATE*)msg.lparam;
					if (info->iItem != -1) {
						ItemClick event(*this, info->iItem, info->iSubItem, info->ptAction);
						onItemClick()(event);
					}
				} break;
				case NM_CUSTOMDRAW : {
					if (onItemPrePaint()) {
						auto info = reinterpret_cast<NMLVCUSTOMDRAW*>(msg.lparam);
						if (info->nmcd.dwDrawStage == CDDS_PREPAINT) {
							msg.result = CDRF_NOTIFYSUBITEMDRAW;
						} else if (info->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
							msg.result = CDRF_NOTIFYSUBITEMDRAW;
						} else if (info->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
							ItemPrePaint event(*this, info);
							onItemPrePaint()(event);
							if (event._fontChanged) {
								msg.result |= CDRF_NEWFONT;
							}
						}
						return;
					}
				} break;
				case NM_DBLCLK : {
					auto info = (NMITEMACTIVATE*)msg.lparam;
					if (info->iItem != -1) {
						ItemDoubleClick event(*this, info->iItem, info->iSubItem, info->ptAction);
						onItemDoubleClick()(event);
					}
				} break;
				case NM_RCLICK : {
					auto info =(NMITEMACTIVATE*)msg.lparam;
					if (info->iItem != -1) {
						ItemRightClick event(*this, info->iItem, info->iSubItem, info->ptAction);
						onItemRightClick()(event);
					}
				} break;
			}
			Control::processMessage(msg);
		} break;
		case WM_MOUSEHOVER : {
			if (hotTrackSelection()) {
				Control::processMessage(msg);
			} else {
				Event event(*this);
				onMouseHover()(event);
			}
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}