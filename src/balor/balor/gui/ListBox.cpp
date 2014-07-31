#include "ListBox.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/ScrollBar.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using std::swap;
using std::vector;
using namespace balor::graphics;


namespace {
static_assert(ListBox::SelectMode::none             == LBS_NOSEL, "Invalid enum value");
static_assert(ListBox::SelectMode::multiple         == LBS_MULTIPLESEL, "Invalid enum value");
static_assert(ListBox::SelectMode::multipleExtended == LBS_EXTENDEDSEL, "Invalid enum value");

static_assert(ListBox::Options::noHScroll        == WS_HSCROLL, "Invalid enum value");
static_assert(ListBox::Options::noVScroll        == WS_VSCROLL, "Invalid enum value");
static_assert(ListBox::Options::scrollBarFixed   == LBS_DISABLENOSCROLL, "Invalid enum value");
static_assert(ListBox::Options::multiColumn      == LBS_MULTICOLUMN, "Invalid enum value");
static_assert(ListBox::Options::noIntegralHeight == LBS_NOINTEGRALHEIGHT, "Invalid enum value");

const int optionsMask = WS_HSCROLL | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_MULTICOLUMN | LBS_NOINTEGRALHEIGHT;
const int optionsXorMask = WS_HSCROLL | WS_VSCROLL;
} // namespace



bool ListBox::SelectMode::_validate(SelectMode value) {
	switch (value) {
		case none :
		case one :
		case multiple :
		case multipleExtended : return true;
		default : return false;
	}
}


bool ListBox::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



ListBox::ListBox()
	: _tabWidth(8), _textColor(Color::controlText()) {
}


ListBox::ListBox(ListBox&& value, bool checkSlicing)
	: Control(move(value))
	, _tabWidth(move(value._tabWidth))
	, _textColor(move(value._textColor))
	, _onSelect(move(value._onSelect))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


ListBox::ListBox(Control& parent, int x, int y, int width, int height, ListBox::SelectMode select, ListBox::Options options)
	: _tabWidth(8), _textColor(Color::controlText()) {
	assert("Invalid ListBox::SelectMode" && SelectMode::_validate(select));
	assert("Invalid ListBox::Options" && Options::_validate(options));
	attachHandle(CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		| LBS_NOTIFY | LBS_HASSTRINGS | LBS_USETABSTOPS | select | (options ^ optionsXorMask)
		, x, y, width, height, parent, nullptr, nullptr, nullptr));
}


ListBox::ListBox(Control& parent, int x, int y, int width, int height, StringRangeArray items, ListBox::SelectMode select, ListBox::Options options)
	: _tabWidth(8), _textColor(Color::controlText()) {
	*this = ListBox(parent, x, y, width, height, select, options);
	this->items(items);
	size(getPreferredSize(width, height));
}


ListBox::~ListBox() {
	destroyHandle();
}


ListBox& ListBox::operator=(ListBox&& value) {
	if (this != &value) {
		this->~ListBox();
		new (this) ListBox(move(value));
	}
	return *this;
}


void ListBox::add(StringRange item) {
	verify(0 <= SendMessageW(handle(), LB_ADDSTRING, 0, (LPARAM)item.c_str()));
}


int ListBox::anchorIndex() const {
	return SendMessageW(handle(), LB_GETANCHORINDEX, 0, 0);
}


void ListBox::anchorIndex(int value) {
	assert("index out of range" && 0 <= value);
	assert("index out of range" && value < count());
	verify(SendMessageW(handle(), LB_SETANCHORINDEX, value, 0) == 0);
}


void ListBox::clear() {
	for (int i = 0, end = count(); i != end; ++i) {
		setItemData(i, UniqueAny()); // LB_RESETCONTENT で WM_DELETEITEM が送られると MSDN に書いてあるのに送られてこないぞ！？
	}
	SendMessageW(handle(), LB_RESETCONTENT, 0, 0);
}


void ListBox::clearSelected() {
	auto mode = selectMode();
	if (mode == SelectMode::one || mode == SelectMode::none) {
		selectedIndex(-1);
	} else {
		verify(SendMessageW(handle(), LB_SETSEL, FALSE, -1) != LB_ERR);
	}
}


int ListBox::count() const {
	int count = SendMessageW(handle(), LB_GETCOUNT, 0, 0);
	assert(0 <= count);
	return count;
}


int ListBox::countPerPage() const {
	return SendMessageW(handle(), LB_GETLISTBOXINFO, 0, 0);
}


Control::Edge ListBox::edge() const {
	return _handle.edge();
}


void ListBox::edge(Control::Edge value) {
	auto oldSize = size();
	_handle.edge(value);
	if (size().height < oldSize.height) {
		size(oldSize.width, oldSize.height + itemHeight() / 2);
	}
}


void ListBox::erase(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	verify(0 <= SendMessageW(handle(), LB_DELETESTRING, index, 0));
}


void ListBox::columnWidth(int value) {
	assert("Can't set columnWidth" && options() & Options::multiColumn);
	SendMessageW(handle(), LB_SETCOLUMNWIDTH, value, 0);
}


int ListBox::firstVisibleIndex() const {
	return SendMessageW(handle(), LB_GETTOPINDEX, 0, 0);
}


void ListBox::firstVisibleIndex(int value) {
	assert("firstVisibleIndex out of range" && 0 <= value);
	assert("firstVisibleIndex out of range" && value < count());
	if (value != firstVisibleIndex()) {
		verify(SendMessageW(handle(), LB_SETTOPINDEX, value, 0) != LB_ERR);
	}
}


int ListBox::focusedIndex() const {
	return SendMessageW(handle(), LB_GETCARETINDEX, 0, 0);
}


void ListBox::focusedIndex(int value) {
	assert("focusedIndex out of range" && 0 <= value);
	assert("focusedIndex out of range" && value < count());
	auto mode = selectMode();
	if (mode == SelectMode::none || mode == SelectMode::one) {
		selectedIndex(value);
	} else {
		verify(SendMessageW(handle(), LB_SETCARETINDEX, value, FALSE) == LB_OKAY);
	}
}


int ListBox::getIndexAt(const Point& position) const {
	return getIndexAt(position.x, position.y);
}


int ListBox::getIndexAt(int x, int y) const {
	return static_cast<short>(LOWORD(SendMessageW(handle(), LB_ITEMFROMPOINT, 0, MAKELPARAM(x, y))));
}


String ListBox::getItem(int index) const {
	StringBuffer buffer;
	getItemToBuffer(buffer, index);
	return move(buffer);
}


void ListBox::getItemToBuffer(StringBuffer& buffer, int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	int length = SendMessageW(handle(), LB_GETTEXTLEN, index, 0);
	assert(0 <= length);
	buffer.reserveAdditionally(length);
	verify(0 <= SendMessageW(handle(), LB_GETTEXT, index, (LPARAM)buffer.end()));
	buffer.expandLength(length);
}


Rectangle ListBox::getItemBounds(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	RECT rect;
	verify(SendMessageW(handle(), LB_GETITEMRECT, index, (LPARAM)&rect) != LB_ERR);
	return rect;
}


bool ListBox::getItemDataIsEmpty(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	return _getItemData(index) == nullptr;
}


Size ListBox::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	int newWidth = width ? 0 : maxItemWidth();
	auto borderSize = sizeFromClientSize(Size(0, 0));
	int newHeight = itemHeight() * count() + borderSize.height;
	return Size(width  ? width  : newWidth
			   ,height ? height : newHeight);
}


void ListBox::insert(int index, StringRange item) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= count());
	verify(0 <= SendMessageW(handle(), LB_INSERTSTRING, index, (LPARAM)item.c_str()));
}


bool ListBox::isSelected(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	auto value = SendMessageW(handle(), LB_GETSEL, index, 0);
	assert(0 <= value);
	return 0 < value;
}


void ListBox::isSelected(int index, bool selected) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	assert("Can't select" && selectMode() != SelectMode::none);
	if (selectMode() == SelectMode::one) {
		if (selected) {
			selectedIndex(index);
		} else {
			if (index == selectedIndex()) {
				selectedIndex(-1);
			}
		}
	} else if (selected != isSelected(index)) {
		verify(SendMessageW(handle(), LB_SETSEL, selected ? TRUE : FALSE, index) != LB_ERR);
		Event event(*this);
		onSelect()(event);
	}
}


int ListBox::itemHeight() const {
	auto height = SendMessageW(handle(), LB_GETITEMHEIGHT, 0, 0);
	assert("Failed to LB_GETITEMHEIGHT" && height != LB_ERR);
	return height;
}


void ListBox::itemHeight(int value) {
	assert("Negative itemHeight" && 0 <= value);
	assert("Too big itemHeight" && value < 256);
	verify(SendMessageW(handle(), LB_SETITEMHEIGHT, 0, value) != LB_ERR);
	invalidate();
}


vector<String> ListBox::items() const {
	vector<String> items;
	int count = this->count();
	items.reserve(count);
	for (int i = 0; i < count; ++i) {
		items.push_back(getItem(i));
	}
	return items;
}


void ListBox::items(StringRangeArray value) {
	clear();
	for (auto i = 0, end = value.length(); i != end; ++i) {
		add(value[i]);
	}
}


int ListBox::maxItemWidth() const {
	Graphics graphics(handle());
	auto font = graphics.font(this->font());
	int width = 0;
	for (auto i = 0, end = count(); i != end; ++i) {
		auto size = graphics.measureText(getItem(i), 10000, 10000
			, Graphics::TextFormat::editControl | Graphics::TextFormat::expandTabs);
		width = max(width, size.width);
	}
	graphics.font(font);
	width += sizeFromClientSize(Size(0, 0)).width;
	return width;
}


Listener<ListBox::Select&>& ListBox::onSelect() { return _onSelect; }


ListBox::Options ListBox::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


void ListBox::select(int first, int last, bool selecting) {
	assert("index out of range" && 0 <= first);
	assert("index out of range" && first < count());
	assert("index out of range" && 0 <= last);
	assert("index out of range" && last < count());
	assert("first over last" && first <= last);
	if (first == last) {
		isSelected(first, selecting);
	} else {
		bool change = false;
		for (auto i = first; i < last; ++i) {
			if (isSelected(i) != selecting) {
				change = true;
				break;
			}
		}
		if (!selecting) {
			swap(first, last);
		}
		verify(SendMessageW(handle(), LB_SELITEMRANGEEX, first, last) != LB_ERR);
		if (change) {
			Event event(*this);
			onSelect()(event);
		}
	}
}


int ListBox::selectedCount() const {
	auto mode = selectMode();
	if (mode == SelectMode::none) {
		return 0;
	} else if (mode == SelectMode::one) {
		return selectedIndex() == -1 ? 0 : 1;
	} else {
		return SendMessageW(handle(), LB_GETSELCOUNT, 0, 0);
	}
}


int ListBox::selectedIndex() const {
	assert("Invalid selectMode" && (selectMode() == SelectMode::none || selectMode() == SelectMode::one));
	return SendMessageW(handle(), LB_GETCURSEL, 0, 0);
}


void ListBox::selectedIndex(int value) { // 表向きは SelectMode::one 専用だが内部で focusedIndex の変更用にも使っている
	assert("index out of range" && -1 <= value);
	assert("index out of range" && value < count());
	assert("Invalid selectMode" && (selectMode() == SelectMode::none || selectMode() == SelectMode::one));
	if (value != selectedIndex()) {
		verify(SendMessageW(handle(), LB_SETCURSEL, value, 0) == value);
		Event event(*this);
		onSelect()(event);
	}
}


std::vector<int, std::allocator<int> > ListBox::selectedIndices() const {
	vector<int> indices;
	const auto mode = selectMode();
	if (mode == SelectMode::one) {
		const int index = selectedIndex();
		if (index != -1) {
			indices.push_back(index);
		}
	} else if (mode != SelectMode::none) {
		const int count = SendMessageW(handle(), LB_GETSELCOUNT, 0, 0);
		assert(count != LB_ERR);
		if (0 < count) {
			indices.resize(count);
			verify(SendMessageW(handle(), LB_GETSELITEMS, count, (LPARAM)indices.data()) == count);
		}
	}
	return indices;
}


void ListBox::selectedIndices(ArrayRange<int> value) {
	assert("Too many indices" && (selectMode() != SelectMode::none || value.size() == 0));
	assert("Too many indices" && (selectMode() != SelectMode::one || value.size() <= 1));
	assert("Too many indices" && value.size() <= count());
	clearSelected();
	for (auto i = value.begin(), end = value.end(); i != end; ++i) {
		assert("index out of range" && 0 <= *i);
		assert("index out of range" && *i < count());
		isSelected(*i, true);
	}
}


void ListBox::setItemData(int index, UniqueAny&& value) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	LPARAM lparam  = SendMessageW(handle(), LB_GETITEMDATA, index, 0);
	assert("Failed to LB_GETITEMDATA" && lparam != LB_ERR);
	UniqueAny& any = *reinterpret_cast<UniqueAny*>(&lparam);
	any = move(value);
	verify(SendMessageW(handle(), LB_SETITEMDATA, index, lparam) != LB_ERR);
}


int ListBox::scrollWidth() const {
	return SendMessageW(handle(), LB_GETHORIZONTALEXTENT, 0, 0);
}


void ListBox::scrollWidth(int value) {
	assert("Negative scrollWidth" && 0 <= value);
	if (value != scrollWidth()) {
		SendMessageW(handle(), LB_SETHORIZONTALEXTENT, value, 0);
	}
}


ListBox::SelectMode ListBox::selectMode() const {
	return static_cast<SelectMode>(_handle.style() & (SelectMode::none | SelectMode::multiple | SelectMode::multipleExtended));
}


int ListBox::tabWidth() const {
	return _tabWidth;
}


void ListBox::tabWidth(int value) {
	assert("Negative tabWidth" && 0 <= value);
	const int width = value * 4;
	verify(SendMessageW(handle(), LB_SETTABSTOPS, 1, (LPARAM)&width));
	_tabWidth = value;
	invalidate();
}


String ListBox::operator[](int index) const {
	return getItem(index);
}


Color ListBox::textColor() const {
	return _textColor;
}


void ListBox::textColor(const Color& value) {
	if (value != _textColor) {
		_textColor = value;
		invalidate();
	}
}


void ListBox::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_DESTROY : {
			clear(); // itemData の開放
			processMessageByDefault(msg);
		} break;
		case wmReflect + WM_CTLCOLORLISTBOX : {
			Graphics graphics((HDC)msg.wparam);
			auto textColor = this->textColor();
			HBRUSH brush = this->brush();
			if (brush || textColor != Color::controlText()) {
				graphics.backTransparent(true);
				graphics.textColor(textColor);
				graphics.brushOrigin(brushOrigin());
				msg.result = (LRESULT)(brush ? brush : static_cast<HBRUSH>(Brush::window()));
			} else {
				Control::processMessage(msg);
			}
		} break;
		case wmReflect + WM_COMMAND : {
			switch (HIWORD(msg.wparam)) {
				case LBN_SELCHANGE : {
					Select event(*this);
					onSelect()(event);
				} break;
			}
			Control::processMessage(msg);
		} break;
		case wmReflect + WM_DELETEITEM : {
			auto info = (DELETEITEMSTRUCT*)msg.lparam;
			UniqueAny& any = *reinterpret_cast<UniqueAny*>(&info->itemData);
			any.~UniqueAny();
			msg.result = TRUE;
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}


void* ListBox::_getItemData(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	auto data = SendMessageW(handle(), LB_GETITEMDATA, index, 0);
	assert("Failed to LB_GETITEMDATA" && data != LB_ERR);
	return reinterpret_cast<void*>(data);
}



	}
}